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
    glDeleteBuffers(1, &Object::transformationBlockBuffer);
    glDeleteBuffers(1, &Light::lightBlockBuffer);
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
    GLsizeiptr lightBufferSize = 16 + 16 + 16;
    glBufferData(GL_UNIFORM_BUFFER, lightBufferSize, 0, GL_DYNAMIC_DRAW);
    Light::lightBlockBuffer = lightBuffer;

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
    postBuffer1.deinit();
    postBuffer2.deinit();
    blitBuffer.deinit();

    bool framebuffersInitialized = false;
    if (deferred) {
        framebuffersInitialized =
                gBuffer.init() &&
                postBuffer1.init(2, true) &&
                postBuffer2.init(2, false) &&
                blitBuffer.init(1, false);
        renderFunction = [&](const std::vector<std::unique_ptr<Object>>& objects, Object* skybox) {
            this->renderDeferred(objects, skybox);
        };
    } else {
        framebuffersInitialized =
                multisampleBuffer.init(2) &&
                postBuffer1.init(1, false) &&
                postBuffer2.init(1, false) &&
                blitBuffer.init(1, false);
        renderFunction = [&](const std::vector<std::unique_ptr<Object>>& objects, Object* skybox) {
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

    GLuint renderedTex = blitBuffer.blitColor(multisampleBuffer.getFramebuffer(), 0);
    renderedTex = renderBloom(multisampleBuffer.getFramebuffer(), renderedTex);
    renderedTex = renderHDR(renderedTex);
    renderedTex = renderPostprocess(renderedTex);
    renderPassthrough(renderedTex);
}

void Renderer::renderDeferred(const std::vector<std::unique_ptr<Object> >& objects, Object* skybox)
{
    setup(&gBuffer, objects);

    postBuffer = &postBuffer1;
    postBuffer->bind();
    renderAmbient();
    renderGBuffer();

    renderShadowmaps();

    postBuffer->bind();
    enableBlending();
    deferredPointLighting();
    deferredDirectionalLighting();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    renderSkybox(skybox);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    GLuint renderedTex = blitBuffer.blitColor(postBuffer->getFramebuffer(), 0);
    renderedTex = renderSSAO(renderedTex);
    renderedTex = renderBloom(postBuffer->getFramebuffer(), renderedTex);
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

    postBuffer1.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postBuffer2.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
                if (!objectInsideFrustum(meshObject)) {
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
            lightComp->setUniforms(light->getPosition(), light->getForward());
            DepthMap* depthMap = depthMapPointers[type][lightNum];
            depthMap->bind();
            depthMap->setUniforms(light->getPosition(), light->getForward());
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
    for (const auto& shaderMeshMap : renderMeshes) {
        shader = resourceManager->getForwardLightShader(shaderMeshMap.first, lightType);
        glUseProgram(shader->getProgram());
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

                for (unsigned int lightNum = 0; lightNum < closestLights[lightType].size(); ++lightNum) {
                    Object* light = closestLights[lightType][lightNum];
                    Light* lightComp = light->getComponent<Light>();
                    lightComp->setUniforms(light->getPosition(), light->getForward());

                    activateShadowMap(lightNum, lightType);

                    meshObject.mesh->render();
                }
            }
        }
    }
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
    shader = resourceManager->getDeferredLightShader(Light::Type::POINT);
    glUseProgram(shader->getProgram());
    setGBufferTextures();

    const std::vector<Object*>& closestPointLights = closestLights[Light::Type::POINT];
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

    shader = resourceManager->getDeferredLightShader(Light::Type::DIRECTIONAL);
    glUseProgram(shader->getProgram());
    setGBufferTextures();
    PostFramebuffer::bindQuadVAO();

    const std::vector<Object*>& closestDirLights = closestLights[Light::Type::DIRECTIONAL];
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
        glUniform1i(ENABLE_SHADOWS_LOCATION, 1);
        depthMapPointers[lightType][lightNum]->activate();
    } else {
        glUniform1i(ENABLE_SHADOWS_LOCATION, 0);
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
    setPostFramebuffer();
    glUseProgram(resourceManager->getShaderProgramByName("ssao"));
    glUniform3fv(SSAO_KERNEL_LOCATION, SSAO_KERNEL_SIZE, glm::value_ptr(ssaoKernel[0]));
    glUniformMatrix4fv(PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(*camera->getProjectionMatrixPointer()));
    renderedTex = postBuffer->draw(std::vector<GLuint>{gBuffer.getViewSpacePositionTexture()});
    return renderedTex;
}

GLuint Renderer::renderBloom(GLuint framebuffer, GLuint renderedTex)
{
    if (camera->getBloomIterations() > 0) {
        setPostFramebuffer();
        GLuint bloomTex = postBuffer->blitColor(framebuffer, 1);
        glUseProgram(resourceManager->getShaderProgramByName("bloom_blur"));
        GLboolean horizontal = true;
        for (unsigned int i = 0; i < camera->getBloomIterations(); ++i) {
            setPostFramebuffer();
            glUniform1i(BLOOM_FILTER_HORIZONTAL, horizontal);
            bloomTex = postBuffer->draw(std::vector<GLuint>(1, bloomTex));
            horizontal = !horizontal;
        }
        setPostFramebuffer();
        glUseProgram(resourceManager->getShaderProgramByName("bloom_blend"));
        renderedTex = postBuffer->draw(std::vector<GLuint>{renderedTex, bloomTex});;
    }
    return renderedTex;
}

GLuint Renderer::renderHDR(GLuint renderedTex)
{
    if (camera->isHDREnabled()) {
        setPostFramebuffer();
        glUseProgram(resourceManager->getShaderProgramByName("hdr"));
        renderedTex = postBuffer->draw(std::vector<GLuint>{renderedTex});
    }
    return renderedTex;
}

GLuint Renderer::renderPostprocess(GLuint renderedTex)
{
    const std::list<Postprocess>& postprocs = camera->getPostprocesses();
    for (auto iter = postprocs.begin(); iter != postprocs.end(); ++iter) {
        iter->bind();
        setPostFramebuffer();
        renderedTex = postBuffer->draw(std::vector<GLuint>(1, renderedTex));
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

void Renderer::setPostFramebuffer()
{
    postBuffer = postBuffer != &postBuffer1 ? &postBuffer1 : &postBuffer2;
}

} // moar
