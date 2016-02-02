#include "resourcemanager.h"

#include <iostream>
#include <fstream>

namespace moar
{

ResourceManager::ResourceManager()
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
    std::string geometry = "";
    std::ifstream shaderInfo(path.c_str());
    if (shaderInfo.is_open()) {
        while (std::getline(shaderInfo, line)) {
            if (line.empty()) {
                continue;
            } else if (line.find(".vert") != std::string::npos) {
                vertex = line;
            } else if (line.find(".frag") != std::string::npos) {
                fragment = line;
            } else if (line.find(".geom") != std::string::npos) {
                geometry = line;
            } else if (!vertex.empty() && !fragment.empty()){
                auto found = shadersByName.find(line);
                if (found == shadersByName.end()) {
                    std::unique_ptr<Shader> shader(new Shader());

                    bool shadersAttached = shader->attachShader(GL_VERTEX_SHADER, std::string(shaderPath + vertex).c_str());
                    shadersAttached = shadersAttached && shader->attachShader(GL_FRAGMENT_SHADER, std::string(shaderPath + fragment).c_str());
                    if (!geometry.empty()) {
                        shadersAttached = shadersAttached && shader->attachShader(GL_GEOMETRY_SHADER, std::string(shaderPath + geometry).c_str());
                    }

                    if (!shadersAttached) {
                        return false;
                    }

                    if (!shader->linkProgram()) {
                        std::cerr << "WARNING: Failed to link shader program: " << line << std::endl;
                        return false;
                    }
                    std::cout << "Created shader: " << line << std::endl;
                    shadersByName.insert(std::make_pair(line, shader.get()));

                    if (fragment.find("_point") != std::string::npos) {
                        ShaderKey key = std::make_pair(vertex.substr(0, vertex.find("_")), Light::POINT);
                        shadersByType.insert(std::make_pair(key, shader.get()));
                    } else if (fragment.find("_dir") != std::string::npos) {
                        ShaderKey key = std::make_pair(vertex.substr(0, vertex.find("_")), Light::DIRECTIONAL);
                        shadersByType.insert(std::make_pair(key, shader.get()));
                    }
                    shaders.push_back(std::move(shader));

                    vertex.clear();
                    geometry.clear();
                    fragment.clear();
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

Material* ResourceManager::createMaterial()
{
    std::unique_ptr<Material> material(new Material());
    auto iter = materials.insert(std::make_pair(material->getId(), std::move(material)));
    return iter.second ? iter.first->second.get() : nullptr;
}

Shader* ResourceManager::getShader(const std::string& name)
{
    auto found = shadersByName.find(name);
    if (found != shadersByName.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader: " << name << std::endl;
        return 0;
    }
}

const Shader* ResourceManager::getShader(const std::string& shader, const Light::Type light)
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
        return iter.second ? iter.first->second.get() : nullptr;
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
            std::cerr << "WARNING: Failed to load one or more cube textures\n";
            return 0;
        }
        auto iter = cubeTextures.insert(std::make_pair(textureKey, std::move(texture)));
        return iter.first->second->getID();
    } else {
        return found->second->getID();
    }
}

Material* ResourceManager::getMaterial(int id)
{
    auto found = materials.find(id);
    if (found != materials.end()) {
        return found->second.get();
    } else {
        std::cerr << "ERROR: Could not find material with id " << id << "\n";
        return 0;
    }
}

} // moar
