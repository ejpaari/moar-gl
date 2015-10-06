#include "resourcemanager.h"

#include <iostream>
#include <fstream>

namespace moar
{

ResourceManager::ResourceManager()
{
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

bool ResourceManager::loadShaders(const std::string& path)
{
    std::string line = "";
    std::string vertex = "";
    std::string fragment = "";
    std::ifstream shaderInfo(path.c_str());
    if (shaderInfo.is_open()) {
        while (std::getline(shaderInfo, line)) {
            if (line.empty()) {
                continue;
            } else if (line.find(".vert") != std::string::npos) {
                vertex = line;
            } else if (line.find(".frag") != std::string::npos) {
                fragment = line;
            } else if (!vertex.empty() && !fragment.empty()){
                auto found = shadersByName.find(line);
                if (found == shadersByName.end()) {
                    std::unique_ptr<Shader> shader(new Shader());
                    std::string vertexShader = shaderPath + vertex;
                    std::string fragmentShader = shaderPath + fragment;

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
                    std::cout << "Created shader: " << line << std::endl;
                    GLuint shaderProgram = shader->getProgram();
                    shadersByName.insert(std::make_pair(line, shaderProgram));

                    if (fragment.find("_point") != std::string::npos) {
                        ShaderKey key = std::make_pair(vertex.substr(0, vertex.find(".vert")), Light::POINT);
                        shadersByType.insert(std::make_pair(key, shaderProgram));
                    } else if (fragment.find("_dir") != std::string::npos) {
                        ShaderKey key = std::make_pair(vertex.substr(0, vertex.find(".vert")), Light::DIRECTIONAL);
                        shadersByType.insert(std::make_pair(key, shaderProgram));
                    }
                    shaders.push_back(std::move(shader));
                } else {
                    std::cerr << "ERROR: Duplicate shader names, can not initialize" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "ERROR: Failed to parse shaders file" << std::endl;
                return false;
            }
        }
        shaderInfo.close();
        return true;
    } else {
        std::cerr << "ERROR: Could not open shader info file: " << path << std::endl;
        return false;
    }
    return true;
}

GLuint ResourceManager::getShader(const std::string& name)
{
    auto found = shadersByName.find(name);
    if (found != shadersByName.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader: " << name << std::endl;
        return 0;
    }
}

GLuint ResourceManager::getShader(const std::string& shader, const Light::Type light)
{
    auto found = shadersByType.find(std::make_pair(shader, light));
    if (found != shadersByType.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader \"" << shader << "\" with light type " << light << std::endl;
        return 0;
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

GLuint ResourceManager::getCubeTexture(std::vector<std::string> textureNames)
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
