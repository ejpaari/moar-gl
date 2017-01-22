#ifndef RENDERER_H
#define RENDERER_H

#include "resourcemanager.h"
#include "rendersettings.h"
#include "object.h"
#include "light.h"
#include "camera.h"
#include "framebuffer.h"
#include "multisamplebuffer.h"
#include "postframebuffer.h"
#include "gbuffer.h"
#include "depthmapdir.h"
#include "depthmappoint.h"
#include "shader.h"
#include "postprocess.h"

#include <unordered_set>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <array>

namespace moar
{

class Renderer
{
public:
    explicit Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    bool init(const RenderSettings* settings, ResourceManager* manager);
    bool setDeferredRenderPath(bool enabled);
    void setCamera(const Camera* camera);
    void render(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void clear();

private:
    struct PostBuffer {
        bool inUse;
        PostFramebuffer framebuffer;
    };

    void renderForward(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void renderDeferred(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void setup(const Framebuffer* fb, const std::vector<std::unique_ptr<Object>>& objects);
    void renderAmbient();
    void renderGBuffer();    
    void renderShadowmaps();
    void forwardLighting(Light::Type lightType);
    void setLightBlockData(Light::Type lightType, int numLights);
    void activateAllShadowMaps(Light::Type lightType, int numLights);
    void setGBufferTextures();
    void deferredPointLighting();
    void deferredDirectionalLighting();
    void activateShadowMap(int lightNum, Light::Type lightType);
    void stencilPass();
    void renderSkybox(Object* skybox = nullptr);
    GLuint renderSSAO(GLuint renderedTex);
    GLuint renderBloom(GLuint renderedTex);
    GLuint renderHDR(GLuint renderedTex);
    GLuint renderPostprocess(GLuint renderedTex);
    void renderPassthrough(GLuint texture);
    void updateObjectContainers(const std::vector<std::unique_ptr<Object>>& objects);
    bool objectInsideFrustum(const Object::MeshObject& mo) const;
    PostFramebuffer* getPostFramebuffer(unsigned int index);
    PostFramebuffer* getFreePostFramebuffer();
    void freeOtherPostFramebuffers(PostFramebuffer* used);

    bool deferred = true;
    std::function<void(const std::vector<std::unique_ptr<Object>>&, Object* skybox)> renderFunction;

    using MaterialId = int;
    using ShaderType = int;
    using MeshMap = std::map<MaterialId, std::vector<Object::MeshObject>>;
    std::map<ShaderType, MeshMap> renderMeshes;
    std::vector<std::vector<Object*>> lights;
    std::array<std::vector<Object*>, Light::Type::NUM_TYPES> closestLights;
    std::unique_ptr<Object> lightSphere;

    const RenderSettings* renderSettings = nullptr;
    ResourceManager* resourceManager = nullptr;
    const Camera* camera = nullptr;

    std::array<DepthMapDirectional, MAX_NUM_SHADOWMAPS> dirDepthMaps;
    std::array<DepthMapPoint, MAX_NUM_SHADOWMAPS> pointDepthMaps;
    std::array<std::vector<DepthMap*>, Light::Type::NUM_TYPES> depthMapPointers;

    MultisampleBuffer multisampleBuffer;
    GBuffer gBuffer;
    std::array<PostBuffer, 3> postBuffers;

    const Shader* shader = nullptr;
    std::unordered_set<unsigned int> objectsInFrustum;
    std::array<glm::vec3, SSAO_KERNEL_SIZE> ssaoKernel;
};

} // moar

#endif // RENDERER_H
