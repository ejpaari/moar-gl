#include "resourcemanager.h"

#include <iostream>
#include <exception>

namespace moar
{

const std::unordered_map<std::pair<Shader::Type, Light::Type>, std::string, ResourceManager::PairHash> ResourceManager::shaderNames =
{
    {std::make_pair(Shader::DIFFUSE, Light::POINT), "diffuse_point"},
    {std::make_pair(Shader::SPECULAR, Light::POINT), "specular_point"},
    {std::make_pair(Shader::NORMAL_MAP, Light::POINT), "normalmap_point"},
    {std::make_pair(Shader::DIFFUSE, Light::DIRECTIONAL), "diffuse_dir"},
    {std::make_pair(Shader::SPECULAR, Light::DIRECTIONAL), "specular_dir"},
    {std::make_pair(Shader::NORMAL_MAP, Light::DIRECTIONAL), "normalmap_dir"}
};

ResourceManager::ResourceManager()
{
    // Todo: Preload resources?
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::setShaderPath(const std::string& path)
{
    shaderPath = path;
}

void ResourceManager::setModelPath(const std::string& path)
{
    modelPath = path;
}

void ResourceManager::setTexturePath(const std::string& path)
{
    texturePath = path;
}

GLuint ResourceManager::getShader(const std::string& name)
{
    auto found = shadersByName.find(name);
    if (found == shadersByName.end()) {
        std::unique_ptr<Shader> shader(new Shader());
        std::string vertexShader = shaderPath + name + ".vert";
        std::string fragmentShader = shaderPath + name + ".frag";

        bool isGood;
        isGood = shader->attachShader(GL_VERTEX_SHADER, vertexShader.c_str());
        if (!isGood) {
            std::cerr << "WARNING: Failed to attach vertex shader: " << vertexShader << std::endl;
            return 0;
        }

        isGood = shader->attachShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());
        if (!isGood) {
            std::cerr << "WARNING: Failed to attach fragment shader: " << fragmentShader << std::endl;
            return 0;
        }

        if (!shader->linkProgram()) {
            return 0;
        }
        std::cout << "Created shader: " << name << std::endl;
        GLuint shaderProgram = shader->getProgram();
        shadersByName.insert(std::make_pair(name, std::move(shader)));
        return shaderProgram;
    } else {
        return found->second->getProgram();
    }
}

GLuint ResourceManager::getShader(const Shader::Type shader, const Light::Type light)
{
    auto typepair = std::make_pair(shader, light);
    auto found = shadersByType.find(typepair );
    if (found == shadersByType.end()) {
        auto shaderName = shaderNames.find(typepair);
        if (shaderName== shaderNames.end()) {
            throw std::logic_error("Shader combination does not exist");
        }
        GLuint shaderProgram = getShader(shaderName->second);
        shadersByType.insert(std::make_pair(typepair, shaderProgram));
        return shaderProgram;
    } else {
        return found->second;
    }
}

Model* ResourceManager::getModel(const std::string& modelName)
{
    auto found = models.find(modelName);
    if (found == models.end()) {
        std::string modelFile = modelPath + modelName;
        std::unique_ptr<Model> model(new Model());
        bool isGood = model->loadModel(modelFile);
        if (!isGood) {
            std::cerr << "WARNING: Failed to load model; " << modelFile << std::endl;
            return nullptr;
        }
        auto iter = models.insert(std::make_pair(modelName, std::move(model)));
        if (iter.second) {
            return iter.first->second.get();
        } else {
            return nullptr;
        }
    } else {
        return found->second.get();
    }
}

GLuint ResourceManager::getTexture(const std::string& textureName)
{
    auto found = textures.find(textureName);
    if (found == textures.end()) {
        std::string textureFile = texturePath + textureName;
        std::unique_ptr<Texture> texture(new Texture());
        bool isGood = texture->load(textureFile);
        if (!isGood) {
            std::cerr << "WARNING: Failed to load texture; " << textureFile << std::endl;
            return 0;
        }
        auto iter = textures.insert(std::pair<std::string, std::unique_ptr<Texture>>(textureName, std::move(texture)));
        return iter.first->second->getID();
    } else {
        return found->second->getID();
    }
}

GLuint ResourceManager::getTexture(std::vector<std::string> textureNames)
{
    std::string textureKey = "";
    for (auto& name : textureNames) {
        textureKey += name;
        name = texturePath + name;
    }
    auto found = cubeTextures.find(textureKey);
    if (found == cubeTextures.end()) {
        std::unique_ptr<Texture> texture(new Texture());
        bool isGood = texture->load(textureNames);
        if (!isGood) {
            std::cerr << "WARNING: Failed to load one or more cube textures" << std::endl;
            return 0;
        }
        auto iter = cubeTextures.insert(std::make_pair(textureKey, std::move(texture)));
        return iter.first->second->getID();
    } else {
        return found->second->getID();
    }
}

} // moar
