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
    void renderForward(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void renderDeferred(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void setup(const Framebuffer* fb, const std::vector<std::unique_ptr<Object>>& objects);    
    void lighting(Light::Type lightType);
    void renderSkybox(Object* skybox = nullptr);
    GLuint renderHDR(GLuint renderedTex);
    void renderPassthrough(GLuint texture);
    void updateObjectContainers(const std::vector<std::unique_ptr<Object>>& objects);
    bool objectInsideFrustum(const Object::MeshObject& mo) const;
    void setPostFramebuffer();

    bool deferred = true;

    using MaterialId = int;
    using ShaderType = int;
    using MeshMap = std::map<MaterialId, std::vector<Object::MeshObject>>;
    std::map<ShaderType, MeshMap> renderMeshes;
    std::vector<std::vector<Object*>> lights;

    const RenderSettings* renderSettings = nullptr;
    ResourceManager* resourceManager = nullptr;
    const Camera* camera = nullptr;
    DepthMapDirectional depthMapDir;
    DepthMapPoint depthMapPoint;
    std::vector<DepthMap*> depthMapPointers;
    MultisampleBuffer multisampleBuffer;
    GBuffer gBuffer;
    PostFramebuffer postBuffer1;
    PostFramebuffer postBuffer2;
    PostFramebuffer blitBuffer;
    PostFramebuffer* postBuffer = nullptr;
    const Shader* shader = nullptr;

    std::unordered_set<unsigned int> objectsInFrustum;
};

} // moar

#endif // RENDERER_H
