#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include "resourcemanager.h"

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

    glm::vec4 clearColor;

    GLuint skyboxShader;
    std::vector<std::string> skyboxTextures;    

    GLuint ambientShader;
    glm::vec3 ambientColor;

    int windowWidth;
    int windowHeight;

private:
    bool loaded;
};

} // moar

#endif // RENDERSETTINGS_H
