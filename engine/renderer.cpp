#include "renderer.h"
#include "common/globals.h"

namespace moar
{

Renderer::Renderer()
{
    depthMapPointers.resize(Light::NUM_TYPES);
    depthMapPointers[Light::DIRECTIONAL] = &depthMapDir;
    depthMapPointers[Light::POINT] = &depthMapPoint;
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

    depthMapDir.setSize(renderSettings->windowWidth, renderSettings->windowHeight);
    if (!depthMapDir.init()) {
        return false;
    }

    depthMapPoint.setSize(1024, 1024);
    if (!depthMapPoint.init()) {
        return false;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    lightSphere.reset(new Object);
    Model* sphereModel = manager->getModel("lowpoly_sphere.obj");
    lightSphere->addComponent<Model>(sphereModel);

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
    } else {
        framebuffersInitialized =
                multisampleBuffer.init(2) &&
                postBuffer1.init(1, false) &&
                postBuffer2.init(1, false) &&
                blitBuffer.init(1, false);
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
    if (deferred) {
        renderDeferred(objects, skybox);
    } else {
        renderForward(objects, skybox);
    }
}

void Renderer::renderForward(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox)
{
    setup(&multisampleBuffer, objects);
    renderAmbient();

    glEnable(GL_BLEND);

    for (int i = 0; i < Light::NUM_TYPES; ++i) {
        forwardLighting(Light::Type(i));
    }

    glDisable(GL_BLEND);

    renderSkybox(skybox);

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
    
    gBuffer.bind();
    for (auto& shaderMeshMap : renderMeshes) {
        shader = resourceManager->getGBufferShader(shaderMeshMap.first);
        glUseProgram(shader->getProgram());
        glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
        glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
        for (auto& meshMap : shaderMeshMap .second) {
            resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
            for (auto& meshObject : meshMap.second) {
                if (!objectInsideFrustum(meshObject)) {
                   continue;
                }
                meshObject.parent->setUniforms(shader);
                meshObject.mesh->render();
                objectsInFrustum.insert(meshObject.mesh->getId());
            }
        }
    }

    glEnable(GL_BLEND);

    Light::Type lightType = Light::POINT;
    DepthMap* depthMap = depthMapPointers[lightType];
    for (auto& light : lights[lightType]) {
        renderShadowmap(lightType, light, depthMap);

        Light* lightComponent = light->getComponent<Light>();
        float r = lightComponent->getRange();
        lightSphere->setScale(glm::vec3(r, r, r));
        lightSphere->setPosition(light->getPosition());
        lightSphere->updateModelMatrix();

        postBuffer->bind();
        stencilPass();

        shader = resourceManager->getShaderByName("deferred_light");
        glUseProgram(shader->getProgram());
        glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
        glUniform2f(SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight);
        glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
        depthMap->activate();
        const std::vector<GLuint>& textures = gBuffer.getTextures();
        for (unsigned int i = 0; i < textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i + 1);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glUniform1i(RENDERED_TEX_LOCATION0 + i, i + 1);
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glDisable(GL_DEPTH_TEST);

        lightComponent->setUniforms(light->getPosition(), light->getForward());
        lightSphere->setUniforms(shader);
        lightSphere->getMeshObjects().front().mesh->render();
    }

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    renderSkybox(skybox);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    GLuint renderedTex = blitBuffer.blitColor(postBuffer->getFramebuffer(), 0);
    renderedTex = renderBloom(postBuffer->getFramebuffer(), renderedTex);
    renderedTex = renderHDR(renderedTex);
    renderedTex = renderPostprocess(renderedTex);
    renderPassthrough(renderedTex);    
}

void Renderer::clear()
{
    renderMeshes.clear();
    lights.clear();
    lights.resize(Light::Type::NUM_TYPES);
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

    shader = renderSettings->ambientShader;
    glUseProgram(shader->getProgram());
    glUniform3f(AMBIENT_LOCATION, renderSettings->ambientColor.x, renderSettings->ambientColor.y, renderSettings->ambientColor.z);
    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap .second) {
            resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
            for (auto& meshObject : meshMap.second) {
                if (!objectInsideFrustum(meshObject)) {
                   continue;
                }
                meshObject.parent->setUniforms(shader);
                meshObject.mesh->render();
                objectsInFrustum.insert(meshObject.mesh->getId());
            }
        }
    }
}

void Renderer::forwardLighting(Light::Type lightType)
{
    DepthMap* depthMap = depthMapPointers[lightType];
    for (auto& light : lights[lightType]) {
        renderShadowmap(lightType, light, depthMap);

        multisampleBuffer.bind();

        for (auto& shaderMeshMap : renderMeshes) {
            shader = resourceManager->getForwardLightShader(shaderMeshMap.first, lightType);
            glUseProgram(shader->getProgram());
            glUniform3f(CAMERA_POS_LOCATION, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
            if (shader->hasUniform(FAR_CLIP_DISTANCE_LOCATION)) {
                glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
            }
            depthMap->activate();
            for (auto& meshMap : shaderMeshMap.second) {
                resourceManager->getMaterial(meshMap.first)->setUniforms(shader);
                for (auto& meshObject : meshMap.second) {
                    if (objectsInFrustum.find(meshObject.mesh->getId()) == objectsInFrustum.end()) {
                        continue;
                    }
                    meshObject.parent->setUniforms(shader);
                    meshObject.mesh->render();
                }
            }
        }
    }
}

void Renderer::renderShadowmap(Light::Type lightType, Object* light, DepthMap* depthMap)
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);

    shader = resourceManager->getDepthMapShader(lightType);
    glUseProgram(shader->getProgram());
    Light* lightComp = light->getComponent<Light>();
    lightComp->setUniforms(light->getPosition(), light->getForward());
    depthMap->bind(light->getPosition(), light->getForward());
    if (lightComp->isShadowingEnabled()) {
        for (auto& shaderMeshMap : renderMeshes) {
            for (auto& meshMap : shaderMeshMap.second) {
                for (auto& meshObject : meshMap.second) {
                    if (meshObject.parent->isShadowCaster()) {
                        meshObject.parent->setUniforms(shader);
                        meshObject.mesh->render();
                    }
                }
            }
        }
    }
}

void Renderer::stencilPass()
{
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    shader = resourceManager->getShaderByName("stencil_pass");
    glUseProgram(shader->getProgram());
    lightSphere->setUniforms(shader);
    lightSphere->getMeshObjects().front().mesh->render();
}

void Renderer::renderSkybox(Object* skybox)
{    
    if (skybox) {
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        shader = renderSettings->skyboxShader;
        glUseProgram(shader->getProgram());
        skybox->setUniforms(shader);
        for (auto& meshObject : skybox->getMeshObjects()) {
            meshObject.material->setUniforms(shader);
            meshObject.mesh->render();
        }
        glCullFace(GL_BACK);
    }
}

GLuint Renderer::renderBloom(GLuint framebuffer, GLuint renderedTex)
{
    if (camera->getBloomIterations() > 0) {
        setPostFramebuffer();
        GLuint bloomTex = postBuffer->blitColor(framebuffer, 1);
        glUseProgram(resourceManager->getShaderByName("bloom_blur")->getProgram());
        GLboolean horizontal = true;
        for (unsigned int i = 0; i < camera->getBloomIterations(); ++i) {
            setPostFramebuffer();
            glUniform1i(BLOOM_FILTER_HORIZONTAL, horizontal);
            bloomTex = postBuffer->draw(std::vector<GLuint>(1, bloomTex));
            horizontal = !horizontal;
        }
        setPostFramebuffer();
        glUseProgram(resourceManager->getShaderByName("bloom_blend")->getProgram());
        renderedTex = postBuffer->draw(std::vector<GLuint>{renderedTex, bloomTex});;
    }
    return renderedTex;
}

GLuint Renderer::renderHDR(GLuint renderedTex)
{
    if (camera->isHDREnabled()) {
        setPostFramebuffer();
        glUseProgram(resourceManager->getShaderByName("hdr")->getProgram());
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

    glUseProgram(resourceManager->getShaderByName("passthrough")->getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(RENDERED_TEX_LOCATION0, 0);

    PostFramebuffer::bindQuadVAO();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Renderer::updateObjectContainers(const std::vector<std::unique_ptr<Object>>& objects)
{
    if (!G_COMPONENT_CHANGED) {
        return;
    }

    for (const auto& obj : objects) {
        for (auto& meshObject : obj->getMeshObjects()) {
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
