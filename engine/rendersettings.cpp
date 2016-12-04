#include "rendersettings.h"

#include <boost/property_tree/exceptions.hpp>
#include <iostream>

namespace moar
{

RenderSettings::RenderSettings()
{    
}

RenderSettings::~RenderSettings()
{
}

bool RenderSettings::loadSettings(const boost::property_tree::ptree& pt, ResourceManager& manager)
{
    try {
        clearColor.r = pt.get<float>("Render.clearColorR");
        clearColor.g = pt.get<float>("Render.clearColorG");
        clearColor.b = pt.get<float>("Render.clearColorB");
        clearColor.a = pt.get<float>("Render.clearColorA");
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        skyboxShader = manager.getShaderByName(pt.get<std::string>("Render.skyboxShader"));
        skyboxTextures.resize(6);

        ambientShader = manager.getShaderByName(pt.get<std::string>("Render.ambientShader"));
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load render settings from the .ini-file\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    loaded = true;
    return true;
}

bool RenderSettings::isLoaded() const
{
    return loaded;
}

} // moar
