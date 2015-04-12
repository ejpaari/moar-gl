#include "resourcemanager.h"

#include <iostream>
#include <utility>

namespace moar
{

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

GLuint ResourceManager::getShader(const std::string& shaderName)
{
    auto found = shaders.find(shaderName);
    if (found == shaders.end()) {
        std::unique_ptr<Shader> shader(new Shader());
        std::string vertexShader = shaderPath + shaderName + ".vert";
        std::string fragmentShader = shaderPath + shaderName + ".frag";

        bool isGood;
        isGood = shader->attachShader(GL_VERTEX_SHADER, vertexShader.c_str());
        if (!isGood) {
            std::cerr << "Failed to attach vertex shader: " << vertexShader << std::endl;
            return 0;
        }

        isGood = shader->attachShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());
        if (!isGood) {
            std::cerr << "Failed to attach fragment shader: " << fragmentShader << std::endl;
            return 0;
        }

        shader->linkProgram();
        std::cout << "Created shader: " << shaderName << std::endl;
        GLuint shaderProgram = shader->getProgram();
        shaders.insert(std::pair<std::string, std::unique_ptr<Shader>>(shaderName, std::move(shader)));        
        return shaderProgram;
    } else {
        return found->second->getProgram();
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
            std::cerr << "Failed to load model: " << modelFile << std::endl;
            return nullptr;
        }
        auto iter = models.insert(std::pair<std::string, std::unique_ptr<Model>>(modelName, std::move(model)));
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
            std::cerr << "Failed to load texture: " << textureFile << std::endl;
            return 0;
        }
        auto iter = textures.insert(std::pair<std::string, std::unique_ptr<Texture>>(textureName, std::move(texture)));
        return iter.first->second->getName();
    } else {
        return found->second->getName();
    }
}

} // moar
