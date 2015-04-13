#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "shader.h"
#include "model.h"
#include "texture.h"

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace moar
{

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    void setShaderPath(const std::string& path) { shaderPath = path; }
    void setModelPath(const std::string& path) { modelPath = path; }
    void setTexturePath(const std::string& path) { texturePath = path; }

    GLuint getShader(const std::string& shaderName);
    Model* getModel(const std::string& modelName);
    GLuint getTexture(const std::string& textureName);

private:
    std::string shaderPath;
    std::string modelPath;
    std::string texturePath;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};

} // moar

#endif // RESOURCEMANAGER_H