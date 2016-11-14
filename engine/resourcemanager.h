#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "shader.h"
#include "light.h"
#include "model.h"
#include "texture.h"
#include "material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <functional>

namespace moar
{

class ResourceManager
{
public:
    ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    void setShaderPath(const std::string& path);
    void setModelPath(const std::string& path);
    void setTexturePath(const std::string& path);
    void setLevelPath(const std::string& path);
    bool loadShaders(const std::string& path);
    void clear();

    Material* createMaterial();

    const Shader* getShader(const std::string& name);
    const Shader* getShader(int shaderType, Light::Type light);
    Model* getModel(const std::string& modelName);
    GLuint getTexture(const std::string& textureName);
    GLuint getCubeTexture(std::vector<std::string> textureNames);
    Material* getMaterial(int id);
    std::string getLevelPath() const;

    void checkMissingTextures() const;

private:
    using ShaderKey = std::pair<int, Light::Type>;

    struct ShaderKeyHash
    {
        size_t operator()(const ShaderKey& key) const
        {
            return std::hash<std::string>()(std::to_string(key.first) + std::to_string(key.second));
        }
    };

    bool loadShader(int shaderType);
    bool loadModel(Model* model, const std::string& file);
    bool loadMaterial(aiMaterial* aMaterial, Material* material);

    std::string shaderPath;
    std::string modelPath;
    std::string texturePath;
    std::string levelPath;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::unordered_map<ShaderKey, Shader*, ShaderKeyHash> shadersByType;
    std::unordered_map<std::string, Shader*> shadersByName;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<Texture>> cubeTextures;
    std::unordered_map<int, std::unique_ptr<Material>> materials;
};

} // moar

#endif // RESOURCEMANAGER_H
