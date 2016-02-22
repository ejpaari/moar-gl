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
                        std::cerr << "WARNING: Failed to link shader program: " << line << "\n";
                        return false;
                    }
                    std::cout << "Created shader: " << line << "\n";
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
                    std::cerr << "ERROR: Duplicate shader names, can not initialize\n";
                    return false;
                }
            } else {
                std::cerr << "ERROR: Failed to parse shaders file\n";
                return false;
            }
        }
        shaderInfo.close();
        return true;
    } else {
        std::cerr << "ERROR: Could not open shader info file: " << path << "\n";
        return false;
    }
    return true;
}

Material* ResourceManager::createMaterial()
{
    std::unique_ptr<Material> material(new Material());
    auto iter = materials.insert(std::make_pair(material->getId(), std::move(material)));
    if (iter.second) {
        return iter.first->second.get();
    } else {
        std::cerr << "WARNING: Could not create material\n";
        return nullptr;
    }
}

const Shader* ResourceManager::getShader(const std::string& name)
{
    auto found = shadersByName.find(name);
    if (found != shadersByName.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader: " << name << "\n";
        return nullptr;
    }
}

const Shader* ResourceManager::getShader(const std::string& shader, const Light::Type light)
{
    auto found = shadersByType.find(std::make_pair(shader, light));
    if (found != shadersByType.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader \"" << shader << "\" with light type " << light << "\n";
        return nullptr;
    }
}

Model* ResourceManager::getModel(const std::string& modelName)
{
    auto found = models.find(modelName);
    if (found == models.end()) {
        std::string modelFile = modelPath + modelName;
        std::unique_ptr<Model> model(new Model());
        bool isGood = loadModel(model.get(), modelFile);
        if (!isGood) {
            std::cerr << "WARNING: Failed to load model; " << modelFile << "\n";
            return nullptr;
        }
        auto iter = models.insert(std::make_pair(modelName, std::move(model)));
        if (iter.second) {
            return iter.first->second.get();
        } else {
            std::cerr << "WARNING: Could not insert model " << modelName << "\n";
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
            std::cerr << "WARNING: Failed to load texture; " << textureFile << "\n";
            return 0;
        }
        auto iter = textures.insert(std::make_pair(textureName, std::move(texture)));
        if (iter.second) {
            return iter.first->second->getID();
        } else {
            std::cerr << "WARNING: Could not insert texture " << textureName << "\n";
            return 0;
        }
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
        if (iter.second) {
            return iter.first->second->getID();
        } else {
            std::cerr << "WARNING: Could not insert cube texture\n";
            return 0;
        }
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

bool ResourceManager::loadModel(Model* model, const std::string& file)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices  |
            aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
    // This gives false memory leaks?
    const aiScene* aScene = importer.ReadFile(file, flags);

    if (aScene) {
        if (aScene->mNumMeshes == 0) {
            std::cerr << "WARNING: No mesh found in the model; " << file << "\n";
            return false;
        }

        for (unsigned int i = 0; i < aScene->mNumMeshes; ++i) {
            const aiMesh* aMesh = aScene->mMeshes[i];
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec2> texCoords;
            std::vector<unsigned int> indices;
            std::unique_ptr<Mesh> mesh(new Mesh());

            for (unsigned int j = 0; j < aMesh->mNumVertices; ++j) {
                glm::vec3 v;
                v.x = aMesh->mVertices[j].x;
                v.y = aMesh->mVertices[j].y;
                v.z = aMesh->mVertices[j].z;
                vertices.push_back(v);
                mesh->checkBoundingBoxLimits(v);

                glm::vec3 n;
                n.x = aMesh->mNormals[j].x;
                n.y = aMesh->mNormals[j].y;
                n.z = aMesh->mNormals[j].z;
                normals.push_back(n);

                if (aMesh->HasTangentsAndBitangents()) {
                    glm::vec3 tan;
                    tan.x = aMesh->mTangents[j].x;
                    tan.y = aMesh->mTangents[j].y;
                    tan.z = aMesh->mTangents[j].z;
                    tangents.push_back(tan);
                }

                if (aMesh->HasTextureCoords(0)) {
                    glm::vec2 t;
                    t.x = aMesh->mTextureCoords[0][j].x;
                    t.y = -aMesh->mTextureCoords[0][j].y;
                    texCoords.push_back(t);
                }
            }

            for (unsigned int j = 0; j < aMesh->mNumFaces; ++j) {
                if (aMesh->mFaces[j].mNumIndices != 3) {
                    std::cerr << "WARNING: Unable to parse model indices; " << file << "\n";
                    return false;
                }
                indices.push_back(aMesh->mFaces[j].mIndices[0]);
                indices.push_back(aMesh->mFaces[j].mIndices[1]);
                indices.push_back(aMesh->mFaces[j].mIndices[2]);
            }

            mesh->setVertices(vertices);
            mesh->setIndices(indices);
            mesh->setNormals(normals);            
            if (aMesh->HasTangentsAndBitangents()) {
                mesh->setTangents(tangents);
            }
            if (aMesh->HasTextureCoords(0)) {
                mesh->setTextureCoordinates(texCoords);
            }
            mesh->calculateCenterPointAndRadius();

            // Todo: Do not reload existing materials.
            aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];
            if (aMaterial) {
                std::unique_ptr<Material> mat(new Material());
                if (loadMaterial(aMaterial, mat.get())) {
                    mesh->setDefaultMaterial(mat.get());
                    auto iter = materials.insert(std::make_pair(mat->getId(), std::move(mat)));
                    if (!iter.second) {
                        std::cerr << "ERROR: Could not insert material.\n";
                    }
                }
            }
            model->addMesh(std::move(mesh));
        }
        std::cout << "Loaded model: " << file << "\n";
        return true;
    } else {
        std::cerr << "WARNING: Failed to read model \"" << file << "\"\n";
        std::cerr << importer.GetErrorString() << "\n";
        return false;
    }
}

bool ResourceManager::loadMaterial(aiMaterial* aMaterial, Material* material)
{
    std::string shaderType;    
    aiTextureType textureType = aiTextureType_DIFFUSE;
    if (aMaterial->GetTextureCount(textureType) > 0) {
        if (!loadTexture(aMaterial, textureType, material, Material::DIFFUSE)) {
            return false;
        }
        shaderType = "diffuse";
    }
    textureType = aiTextureType_HEIGHT;
    if (aMaterial->GetTextureCount(textureType) > 0) {
        if (!loadTexture(aMaterial, textureType, material, Material::NORMAL)) {
            return false;
        }
        shaderType = "normalmap";
    }
    textureType = aiTextureType_DISPLACEMENT;
    if (aMaterial->GetTextureCount(textureType) > 0) {
        if (!loadTexture(aMaterial, textureType, material, Material::BUMP)) {
            return false;
        }
        shaderType = "bumpmap";
    }

    if (shaderType.empty()) {
        return false;
    }
    material->setShaderType(shaderType);
    return true;
}

bool ResourceManager::loadTexture(aiMaterial* aMaterial, aiTextureType aType, Material* material, Material::TextureType type)
{
    aiString path;
    aMaterial->GetTexture(aType, 0, &path);
    GLuint tex = getTexture(std::string(path.C_Str()));
    if (tex == 0) {
        return false;
    }
    material->setTexture(tex, type, GL_TEXTURE_2D);
    return true;
}

} // moar
