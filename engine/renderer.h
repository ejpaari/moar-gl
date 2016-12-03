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
    void setCamera(const Camera* camera);
    void renderForward(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void renderDeferred(const std::vector<std::unique_ptr<Object>>& objects, Object* skybox = nullptr);
    void clear();

private:
    void renderSetup(const Framebuffer* fb, const std::vector<std::unique_ptr<Object>>& objects);
    void lighting(Light::Type lightType);
    void updateObjectContainers(const std::vector<std::unique_ptr<Object>>& objects);
    bool objectInsideFrustum(const Object::MeshObject& mo) const;
    void setPostFramebuffer();

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
    MultisampleBuffer multisampleBuffer;
    GBuffer gBuffer;
    PostFramebuffer postBuffer1;
    PostFramebuffer postBuffer2;
    PostFramebuffer blitBuffer1;
    PostFramebuffer blitBuffer2;
    PostFramebuffer* postBuffer = nullptr;
    const Shader* shader = nullptr;
    Postprocess passthrough;

    std::unordered_set<unsigned int> objectsInFrustum;
};

} // moar

#endif // RENDERER_H
