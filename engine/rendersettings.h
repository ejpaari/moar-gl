#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include "resourcemanager.h"
#include "shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <string>

namespace moar
{

class RenderSettings
{
public:
    explicit RenderSettings();
    ~RenderSettings();
    RenderSettings(const RenderSettings&) = delete;
    RenderSettings(RenderSettings&&) = delete;
    RenderSettings& operator=(const RenderSettings&) = delete;
    RenderSettings& operator=(RenderSettings&&) = delete;

    bool loadSettings(const boost::property_tree::ptree& pt, ResourceManager& manager);
    bool isLoaded() const;

    glm::vec4 clearColor = glm::vec4(0.0f, 0.2f, 0.0f, 1.0f);

    const Shader* skyboxShader = nullptr;
    std::vector<std::string> skyboxTextures;    

    const Shader* ambientShader = nullptr;
    glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);

    int windowWidth = 800;
    int windowHeight = 600;

    int directionalShadowMapWidth = 1024;
    int directionalShadowMapHeight = 768;
    int pointShadowMapSize = 512;

private:
    bool loaded = false;
};

} // moar

#endif // RENDERSETTINGS_H
