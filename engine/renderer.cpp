#include "renderer.h"
#include "common/globals.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include <random>
#include <utility>
#include <algorithm>

namespace moar
{

namespace
{

const GLint COLOR_ELEMENT_SIZE = 16;
const GLint PROJECTION_ELEMENT_SIZE = 64;
constexpr GLintptr COLOR_OFFSET = 0;
constexpr GLintptr POS_OFFSET = MAX_NUM_LIGHTS_PER_TYPE * COLOR_ELEMENT_SIZE;
constexpr GLintptr FORWARD_OFFSET = MAX_NUM_LIGHTS_PER_TYPE * COLOR_ELEMENT_SIZE * 2;

void enableBlending()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

} // anonymous

Renderer::Renderer()
{
    for (int i = 0; i < MAX_NUM_SHADOWMAPS; ++i) {
        depthMapPointers[Light::POINT].push_back(&pointDepthMaps[i]);
        depthMapPointers[Light::DIRECTIONAL].push_back(&dirDepthMaps[i]);
    }

    std::uniform_real_distribution<GLfloat> distribution(-1.0, 1.0);
    std::default_random_engine random;

    float kernelSize = static_cast<float>(SSAO_KERNEL_SIZE);
    for (int i = 0; i < SSAO_KERNEL_SIZE; ++i) {
        glm::vec3 sample(distribution(random), distribution(random), distribution(random));
        sample = glm::normalize(sample) * 0.05f;
        float scale = static_cast<float>(i) / kernelSize;
        sample *= glm::lerp(0.1f, 1.0f, scale * scale); // Move sample closer to center
        ssaoKernel[i] = sample;
    }
}

Renderer::~Renderer()
{    
    glDeleteBuffers(1, &Light::lightBlockBuffer);
    glDeleteBuffers(1, &Light::lightProjectionBlockBuffer);
    glDeleteBuffers(1, &Object::transformationBlockBuffer);
    PostFramebuffer::uninitQuad();
}

bool Renderer::init(const RenderSettings* settings, ResourceManager* manager)
{
    if (settings && manager) {
        renderSettings = settings;
        resourceManager = manager;
    } else {
        std::cerr << "ERROR: Invalid render settings\n";
        return false;
    }

    lightSphere.reset(new Object);
    Model* sphereModel = manager->getModel("lowpoly_sphere.obj");
    lightSphere->addComponent<Model>(sphereModel);

    for (int i = 0; i < MAX_NUM_SHADOWMAPS; ++i) {
        pointDepthMaps[i].setSize(renderSettings->pointShadowMapSize, renderSettings->pointShadowMapSize);
        if (!pointDepthMaps[i].init()) {
            return false;
        }
        dirDepthMaps[i].setSize(renderSettings->directionalShadowMapWidth, renderSettings->directionalShadowMapHeight);
        if (!dirDepthMaps[i].init()) {
            return false;
        }
    }

    Framebuffer::setSize(renderSettings->windowWidth, renderSettings->windowHeight);
    PostFramebuffer::initQuad();

    GLuint lightBuffer;
    glGenBuffers(1, &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
    GLsizeiptr lightBufferSize = (COLOR_ELEMENT_SIZE * 3) * MAX_NUM_LIGHTS_PER_TYPE;
    glBufferData(GL_UNIFORM_BUFFER, lightBufferSize, 0, GL_DYNAMIC_DRAW);
    Light::lightBlockBuffer = lightBuffer;

    GLuint lightProjectionBuffer;
    glGenBuffers(1, &lightProjectionBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightProjectionBuffer);
    GLsizeiptr lightProjectionBufferSize = PROJECTION_ELEMENT_SIZE * MAX_NUM_LIGHTS_PER_TYPE;
    glBufferData(GL_UNIFORM_BUFFER, lightProjectionBufferSize, 0, GL_DYNAMIC_DRAW);
    Light::lightProjectionBlockBuffer = lightProjectionBuffer;

    GLuint transformationBuffer;
    glGenBuffers(1, &transformationBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    GLsizeiptr transformationBufferSize = 4 * sizeof(*Object::projection);
    glBufferData(GL_UNIFORM_BUFFER, transformationBufferSize, 0, GL_DYNAMIC_DRAW);
    Object::transformationBlockBuffer = transformationBuffer;

    glEnable(GL_MULTISAMPLE);

    return true;
}

bool Renderer::setDeferredRenderPath(bool enabled)
{
    if (deferred == enabled) {
        if (deferred && gBuffer.getFramebuffer() != 0) {
            return true;
        } else if (!deferred && multisampleBuffer.getFramebuffer() != 0) {
            return true;
        }
    }
    deferred = enabled;

    gBuffer.deinit();
    multisampleBuffer.deinit();
    for (auto& postBuffer : postBuffers) {
        postBuffer.framebuffer.deinit();
    }

    bool framebuffersInitialized = false;
    if (deferred) {
        framebuffersInitialized = gBuffer.init() &&
                postBuffers[0].framebuffer.init(1, true) &&
                postBuffers[1].framebuffer.init(1, false) &&
                postBuffers[2].framebuffer.init(1, false);
        renderFunction = [&] (const std::vector<std::unique_ptr<Object>>& objects, Object* skybox) {
            this->renderDeferred(objects, skybox);
        };
    } else {
        framebuffersInitialized =
                multisampleBuffer.init(1) &&
                postBuffers[0].framebuffer.init(1, false) &&
                postBuffers[1].framebuffer.init(1, false) &&
                postBuffers[2].framebuffer.init(1, false);
        renderFunction = [&] (const std::vector<std::unique_ptr<Object>>& objects, Object* skybox) {
            this->renderForward(objects, skybox);
        };
    }

    if (!framebuffersInitialized) {
        std::cerr << "ERROR: Framebuffer status is incomplete\n";
        return false;
    }
    return true;
}

void Renderer::setCamera(const Camera* camera)
{
    this->camera = camera;
}

void Renderer::render(const std::vector<std::unique_ptr<Object> >& objects, Object* skybox)
{
    renderFunction(objects, skybox);
}

void Renderer::clear()
{
    renderMeshes.clear();
    lights.clear();
    lights.resize(Light::Type::NUM_TYPES);
}

void Renderer::renderForward(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox)
{
    setup(&multisampleBuffer, objects);

    glDisable(GL_STENCIL_TEST);

    renderAmbient();
    renderShadowmaps();
    enableBlending();

    for (int i = 0; i < Light::NUM_TYPES; ++i) {
        forwardLighting(Light::Type(i));
    }

    glDisable(GL_BLEND);

    renderSkybox(skybox);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    PostFramebuffer* buffer = getFreePostFramebuffer();
    GLuint renderedTex = buffer->blitColor(multisampleBuffer.getFramebuffer(), 0);
    renderedTex = renderBloom(renderedTex);
    renderedTex = renderHDR(renderedTex);
    renderedTex = renderPostprocess(renderedTex);
    renderPassthrough(renderedTex);
}

void Renderer::renderDeferred(const std::vector<std::unique_ptr<Object> >& objects, Object* skybox)
{
    setup(&gBuffer, objects);

    PostFramebuffer* buffer = getPostFramebuffer(0);
    buffer->bind();
    renderAmbient();
    renderGBuffer();

    renderShadowmaps();

    buffer->bind();
    enableBlending();
    deferredPointLighting();
    deferredDirectionalLighting();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    renderSkybox(skybox);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    GLuint renderedTex = buffer->getRenderedTex(0);
    renderedTex = renderSSAO(renderedTex);
    renderedTex = renderBloom(renderedTex);
    renderedTex = renderHDR(renderedTex);
    renderedTex = renderPostprocess(renderedTex);
    renderPassthrough(renderedTex);
}

void Renderer::setup(const Framebuffer* fb, const std::vector<std::unique_ptr<Object>>& objects)
{
    if (!camera) {
        std::cerr << "WARNING: Camera not set for renderer\n";
        return;
    }
    updateObjectContainers(objects);
    objectsInFrustum.clear();
    Object::setViewMatrixUniform();

    glDepthMask(GL_TRUE);

    for (auto& postBuffer : postBuffers) {
        postBuffer.inUse = false;
        postBuffer.framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    fb->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderAmbient()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    shader = renderSettings->ambientShader;
    glUseProgram(shader->getProgram());
    glUniform3f(AMBIENT_LOCATION, renderSettings->ambientColor.x, renderSettings->ambientColor.y, renderSettings->ambientColor.z);
    for (const auto& shaderMeshMap : renderMeshes) {
        for (const auto& meshMap : shaderMeshMap .second) {
            resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
            for (const auto& meshObject : meshMap.second) {
                if (!objectInsideFrustum(meshObject)) {
                   continue;
                }
                meshObject.parent->setUniforms();
                meshObject.mesh->render();
                objectsInFrustum.insert(meshObject.mesh->getId());
            }
        }
    }
}

void Renderer::renderGBuffer()
{
    gBuffer.bind();
    for (const auto& shaderMeshMap : renderMeshes) {
        shader = resourceManager->getGBufferShader(shaderMeshMap.first);
        glUseProgram(shader->getProgram());
        glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
        glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
        for (const auto& meshMap : shaderMeshMap .second) {
            resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
            for (const auto& meshObject : meshMap.second) {
                if (objectsInFrustum.find(meshObject.mesh->getId()) == objectsInFrustum.end()) {
                    continue;
                }
                meshObject.parent->setUniforms();
                meshObject.mesh->render();
            }
        }
    }
}

void Renderer::renderShadowmaps()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (int type = 0; type < Light::Type::NUM_TYPES; ++type) {
        shader = resourceManager->getDepthMapShader(Light::Type(type));
        glUseProgram(shader->getProgram());
        int numShadowmaps = std::min(MAX_NUM_SHADOWMAPS, static_cast<int>(closestLights[type].size()));
        for (int lightNum = 0; lightNum < numShadowmaps; ++lightNum) {
            Object* light = closestLights[type][lightNum];
            Light* lightComp = light->getComponent<Light>();
            if (type != Light::Type::DIRECTIONAL) {
                lightComp->setUniforms(light->getPosition(), light->getForward());
            }
            DepthMap* depthMap = depthMapPointers[type][lightNum];
            depthMap->bind();
            depthMap->updateUniformValues(light->getPosition(), light->getForward());
            depthMap->setUniforms();
            glClear(GL_DEPTH_BUFFER_BIT);
            if (lightComp->isShadowingEnabled()) {
                for (const auto& shaderMeshMap : renderMeshes) {
                    for (const auto& meshMap : shaderMeshMap.second) {
                        for (const auto& meshObject : meshMap.second) {
                            if (meshObject.parent->isShadowCaster()) {
                                meshObject.parent->setUniforms();
                                meshObject.mesh->render();
                            }
                        }
                    }
                }
            }
        }
    }
}

void Renderer::forwardLighting(Light::Type lightType)
{
    multisampleBuffer.bind();
    int numLights = static_cast<int>(closestLights[lightType].size());

    setLightBlockData(lightType, numLights);

    for (const auto& shaderMeshMap : renderMeshes) {
        shader = resourceManager->getForwardLightShader(shaderMeshMap.first, lightType);
        glUseProgram(shader->getProgram());

        activateAllShadowMaps(lightType, numLights);

        glUniform1i(NUM_LIGHTS_LOCATION, numLights);
        glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
        if (shader->hasUniform(FAR_CLIP_DISTANCE_LOCATION)) {
            glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
        }
        for (const auto& meshMap : shaderMeshMap.second) {
            resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
            for (const auto& meshObject : meshMap.second) {
                if (objectsInFrustum.find(meshObject.mesh->getId()) == objectsInFrustum.end()) {
                    continue;
                }
                meshObject.parent->setUniforms();
                meshObject.mesh->render();
            }
        }
    }
}

void Renderer::setLightBlockData(Light::Type lightType, int numLights)
{
    GLintptr offset = 0;
    glBindBuffer(GL_UNIFORM_BUFFER, Light::lightBlockBuffer);

    for (int lightNum = 0; lightNum < numLights; ++lightNum) {
        Object* light = closestLights[lightType][lightNum];
        Light* lightComp = light->getComponent<Light>();

        glBufferSubData(GL_UNIFORM_BUFFER, COLOR_OFFSET + offset, 16, glm::value_ptr(lightComp->getColor()));
        glBufferSubData(GL_UNIFORM_BUFFER, POS_OFFSET + offset, 12, glm::value_ptr(light->getPosition()));
        glBufferSubData(GL_UNIFORM_BUFFER, FORWARD_OFFSET  + offset, 12, glm::value_ptr(light->getForward()));
        offset += COLOR_ELEMENT_SIZE;
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, Light::lightBlockBuffer);

    if (lightType == Light::DIRECTIONAL) {
        GLintptr offset = 0;
        glBindBuffer(GL_UNIFORM_BUFFER, Light::lightProjectionBlockBuffer);
        for (int lightNum = 0; lightNum < numLights; ++lightNum) {
            const glm::mat4& projMat = dirDepthMaps[lightNum].getLightSpaceMatrix();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, PROJECTION_ELEMENT_SIZE, glm::value_ptr(projMat));
            offset += PROJECTION_ELEMENT_SIZE;
        }
        glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_PROJECTION_BINDING_POINT, Light::lightProjectionBlockBuffer);
    }
}

void Renderer::activateAllShadowMaps(Light::Type lightType, int numLights)
{
    numLights = std::min(numLights, MAX_NUM_SHADOWMAPS);
    std::vector<GLint> enableShadows(MAX_NUM_LIGHTS_PER_TYPE, 0);
    for (int lightNum = 0; lightNum < numLights; ++lightNum) {
        enableShadows[lightNum] = 1;
        DepthMap* depthMap = depthMapPointers[lightType][lightNum];

        glActiveTexture(GL_TEXTURE5 + lightNum);
        glBindTexture(depthMap->getType(), depthMap->getTexture());
        int location = shader->getShadowMapLocation(lightNum);
        glUniform1i(location, 5 + lightNum);
    }
    glUniform1iv(ENABLE_SHADOWS_LOCATION, numLights, &enableShadows[0]);
}

void Renderer::setGBufferTextures()
{
    glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
    glUniform2f(SCREEN_SIZE_LOCATION, static_cast<float>(renderSettings->windowWidth), static_cast<float>(renderSettings->windowHeight));
    glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
    const std::vector<GLuint>& textures = gBuffer.getDeferredTextures();
    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(RENDERED_TEX_LOCATION1 + i, i + 1);
    }
}

void Renderer::deferredPointLighting()
{
    const std::vector<Object*>& closestPointLights = closestLights[Light::Type::POINT];
    if (closestPointLights.empty()) {
        return;
    }

    shader = resourceManager->getDeferredLightShader(Light::Type::POINT);
    glUseProgram(shader->getProgram());
    setGBufferTextures();

    for (unsigned int lightNum = 0; lightNum < closestPointLights.size(); ++lightNum) {
        Object* light = closestPointLights[lightNum];
        Light* lightComponent = light->getComponent<Light>();
        float r = lightComponent->getRange();
        lightSphere->setScale(glm::vec3(r, r, r));
        lightSphere->setPosition(light->getPosition());
        lightSphere->updateModelMatrix();

        stencilPass();

        glUseProgram(shader->getProgram());
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glDisable(GL_DEPTH_TEST);

        activateShadowMap(lightNum, Light::Type::POINT);
        lightComponent->setUniforms(light->getPosition(), light->getForward());
        lightSphere->setUniforms();
        lightSphere->getMeshObjects().front().mesh->render();
    }
}

void Renderer::deferredDirectionalLighting()
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    const std::vector<Object*>& closestDirLights = closestLights[Light::Type::DIRECTIONAL];
    if (closestDirLights.empty()) {
        return;
    }

    shader = resourceManager->getDeferredLightShader(Light::Type::DIRECTIONAL);
    glUseProgram(shader->getProgram());
    setGBufferTextures();
    PostFramebuffer::bindQuadVAO();

    for (unsigned int lightNum = 0; lightNum < closestDirLights.size(); ++lightNum) {
        activateShadowMap(lightNum, Light::Type::DIRECTIONAL);
        Object* light = closestDirLights[lightNum];
        Light* lightComponent = light->getComponent<Light>();
        lightComponent->setUniforms(light->getPosition(), light->getForward());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Renderer::activateShadowMap(int lightNum, Light::Type lightType)
{
    if (lightNum < MAX_NUM_SHADOWMAPS) {
        glUniform1i(ENABLE_SHADOW_LOCATION, 1);
        depthMapPointers[lightType][lightNum]->activate();
    } else {
        glUniform1i(ENABLE_SHADOW_LOCATION, 0);
    }

}

void Renderer::stencilPass()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    glUseProgram(resourceManager->getShaderProgramByName("stencil_pass"));
    lightSphere->setUniforms();
    lightSphere->getMeshObjects().front().mesh->render();
}

void Renderer::renderSkybox(Object* skybox)
{
    if (skybox) {
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        shader = renderSettings->skyboxShader;
        glUseProgram(shader->getProgram());
        skybox->setUniforms();
        for (const auto& meshObject : skybox->getMeshObjects()) {
            meshObject.material->setUniforms(shader);
            meshObject.mesh->render();
        }
    }
}

GLuint Renderer::renderSSAO(GLuint renderedTex)
{
    if (camera->isSSAOEnabled()) {
        PostFramebuffer* buffer1 = getFreePostFramebuffer();
        PostFramebuffer* buffer2 = getFreePostFramebuffer();

        glUseProgram(resourceManager->getShaderProgramByName("ssao"));
        glUniform3fv(SSAO_KERNEL_LOCATION, SSAO_KERNEL_SIZE, glm::value_ptr(ssaoKernel[0]));
        glUniformMatrix4fv(PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(*camera->getProjectionMatrixPointer()));
        GLuint ssaoTex = buffer1->draw(std::vector<GLuint>{gBuffer.getViewSpacePositionTexture()});

        glUseProgram(resourceManager->getShaderProgramByName("ssao_apply"));
        renderedTex = buffer2->draw(std::vector<GLuint>{renderedTex, ssaoTex});
        freeOtherPostFramebuffers(buffer2);
    }
    return renderedTex;
}

GLuint Renderer::renderBloom(GLuint renderedTex)
{
    if (camera->getBloomIterations() > 0) {
        PostFramebuffer* buffer1 = getFreePostFramebuffer();
        PostFramebuffer* buffer2 = getFreePostFramebuffer();
        PostFramebuffer* buffer = buffer1;

        auto switchBuffer = [&] () {
            buffer = buffer == buffer1 ? buffer2 : buffer1;
        };

        glUseProgram(resourceManager->getShaderProgramByName("bloom_generate"));
        GLuint bloomTex = buffer->draw(std::vector<GLuint>{renderedTex});

        glUseProgram(resourceManager->getShaderProgramByName("bloom_blur"));
        GLboolean horizontal = true;
        for (unsigned int i = 0; i < camera->getBloomIterations(); ++i) {
            switchBuffer();
            glUniform1i(BLOOM_FILTER_HORIZONTAL, horizontal);
            bloomTex = buffer->draw(std::vector<GLuint>{bloomTex});
            horizontal = !horizontal;
        }

        switchBuffer();
        glUseProgram(resourceManager->getShaderProgramByName("bloom_blend"));
        renderedTex = buffer->draw(std::vector<GLuint>{renderedTex, bloomTex});
        freeOtherPostFramebuffers(buffer);
    }
    return renderedTex;
}

GLuint Renderer::renderHDR(GLuint renderedTex)
{
    if (camera->isHDREnabled()) {
        PostFramebuffer* buffer = getFreePostFramebuffer();
        glUseProgram(resourceManager->getShaderProgramByName("hdr"));
        renderedTex = buffer->draw(std::vector<GLuint>{renderedTex});
        freeOtherPostFramebuffers(buffer);
    }
    return renderedTex;
}

GLuint Renderer::renderPostprocess(GLuint renderedTex)
{
    const std::list<Postprocess>& postprocs = camera->getPostprocesses();
    for (auto iter = postprocs.begin(); iter != postprocs.end(); ++iter) {
        iter->bind();
        PostFramebuffer* buffer = getFreePostFramebuffer();
        renderedTex = buffer->draw(std::vector<GLuint>(1, renderedTex));
        freeOtherPostFramebuffers(buffer);
    }
    return renderedTex;
}

void Renderer::renderPassthrough(GLuint texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(resourceManager->getShaderProgramByName("passthrough"));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(RENDERED_TEX_LOCATION0, 0);

    PostFramebuffer::bindQuadVAO();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::updateObjectContainers(const std::vector<std::unique_ptr<Object>>& objects)
{
    glm::vec3 camPos = camera->getPosition();
    for (int type = 0; type < Light::Type::NUM_TYPES; ++type) {
        closestLights[type].clear();
        std::map<float, Object*> lightsByDistance;
        for (const auto light : lights[type]) {
            glm::vec3 temp = camPos - light->getPosition();
            float distance = glm::dot(temp, temp);
            lightsByDistance.emplace(distance, light);
        }
        for (const auto& kv : lightsByDistance) {
            closestLights[type].push_back(kv.second);
        }
    }

    if (!G_COMPONENT_CHANGED) {
        return;
    }

    for (const auto& obj : objects) {
        for (const auto& meshObject : obj->getMeshObjects()) {
            int shaderType = meshObject.material->getShaderType();
            MaterialId materialId = meshObject.material->getId();
            std::vector<Object::MeshObject>& meshes = renderMeshes[shaderType][materialId];
            if (std::find(meshes.begin(), meshes.end(), meshObject) == meshes.end()) {
                meshes.push_back(meshObject);
            }
        }
        if (obj->hasComponent<Light>()) {
            Light::Type type = obj->getComponent<Light>()->getLightType();
            std::vector<Object*>& objs = lights[type];
            if (std::find(objs.begin(), objs.end(), obj.get()) == objs.end()) {
                lights[type].push_back(obj.get());
            }
        }
    }

    auto noLightComponent = [] (Object* obj) { return !obj->hasComponent<Light>(); };
    for (auto& objs : lights) {
        objs.erase(std::remove_if(objs.begin(), objs.end(), noLightComponent), objs.end());
    }

    auto noParent = [] (Object::MeshObject mo) { return mo.parent == nullptr; };
    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap.second) {
            std::vector<Object::MeshObject>& meshes = meshMap.second;
            meshes.erase(std::remove_if(meshes.begin(), meshes.end(), noParent), meshes.end());
        }
    }
    G_COMPONENT_CHANGED = false;

    for (int i = 0; i < Light::NUM_TYPES; ++i) {
        if (lights[i].size() > MAX_NUM_LIGHTS_PER_TYPE) {
            std::cerr << "ERROR: There are more lights than the constant MAX_NUM_LIGHTS_PER_TYPE: "
                      << MAX_NUM_LIGHTS_PER_TYPE << "\n";
        }
    }
}

bool Renderer::objectInsideFrustum(const Object::MeshObject& mo) const
{
    glm::vec3 point = mo.mesh->getCenterPoint();
    point = glm::vec3((*Object::view) * mo.parent->getModelMatrix() * glm::vec4(point.x, point.y, point.z, 1.0f));
    glm::vec3 scale = mo.parent->getScale();
    float scaleMultiplier = std::max(std::max(scale.x, scale.y), scale.z);
    float radius = mo.mesh->getBoundingRadius() * scaleMultiplier ;
    return camera->sphereInsideFrustum(point, radius);
}

PostFramebuffer* Renderer::getPostFramebuffer(unsigned int index)
{
    PostBuffer& buffer = postBuffers[index];
    buffer.inUse = true;
    return &buffer.framebuffer;
}

PostFramebuffer* Renderer::getFreePostFramebuffer()
{
    for (auto& postBuffer : postBuffers) {
        if (!postBuffer.inUse) {
            postBuffer.inUse = true;
            return &postBuffer.framebuffer;
        }
    }
    std::cerr << "ERROR: No free post framebuffers\n";
    return nullptr;
}

void Renderer::freeOtherPostFramebuffers(PostFramebuffer* used)
{
    for (auto& postBuffer : postBuffers) {
        if (used != &postBuffer.framebuffer) {
            postBuffer.inUse = false;
        }
    }
}

} // moar
