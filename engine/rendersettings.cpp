#include "rendersettings.h"

#include <boost/property_tree/exceptions.hpp>
#include <iostream>

namespace moar
{

RenderSettings::RenderSettings()
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

        skyboxShader = manager.getShader(pt.get<std::string>("Render.skyboxShader"));
        skyboxTextures.resize(6);
        skyboxTextures.at(0) = pt.get<std::string>("Render.skyboxTexPosX");
        skyboxTextures.at(1) = pt.get<std::string>("Render.skyboxTexNegX");
        skyboxTextures.at(2) = pt.get<std::string>("Render.skyboxTexPosY");
        skyboxTextures.at(3) = pt.get<std::string>("Render.skyboxTexNegY");
        skyboxTextures.at(4) = pt.get<std::string>("Render.skyboxTexPosZ");
        skyboxTextures.at(5) = pt.get<std::string>("Render.skyboxTexNegZ");

        ambientShader = manager.getShader(pt.get<std::string>("Render.ambientShader"));
        ambientColor.r = pt.get<float>("Render.ambientColorR");
        ambientColor.g = pt.get<float>("Render.ambientColorG");
        ambientColor.b = pt.get<float>("Render.ambientColorB");
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
