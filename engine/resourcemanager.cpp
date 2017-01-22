#include "resourcemanager.h"
#include "common/globals.h"
#include "common/typemappings.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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
    Shader::loadCommonShaderCode(GL_VERTEX_SHADER, path + "/" + COMMON_VERTEX_FILE);
    Shader::loadCommonShaderCode(GL_FRAGMENT_SHADER, path + "/" + COMMON_FRAGMENT_FILE);
    std::stringstream ss;
    ss << "const int SSAO_KERNEL_SIZE = " << SSAO_KERNEL_SIZE << ";\n"
       << "const int MAX_NUM_SHADOWMAPS = " << MAX_NUM_SHADOWMAPS  << ";\n"
       << "const int MAX_NUM_LIGHTS_PER_TYPE = " << MAX_NUM_LIGHTS_PER_TYPE  << ";\n\n";
    Shader::addCommonShaderCode(GL_FRAGMENT_SHADER, ss.str());
}

void ResourceManager::setModelPath(const std::string& path)
{
    modelPath = path;
}

void ResourceManager::setTexturePath(const std::string& path)
{
    texturePath = path;
}

void ResourceManager::setLevelPath(const std::string& path)
{
    levelPath = path;
}

bool ResourceManager::loadShaderFiles(const std::string& path)
{
    std::string line = "";
    std::string vertex = "";
    std::string fragment = "";
    std::string geometry = "";
    std::string name = "";
    std::ifstream shaderInfo(path.c_str());
    if (shaderInfo.is_open()) {
        while (std::getline(shaderInfo, line)) {
            if (line.find(".vert") != std::string::npos) {
                vertex = line;
                name = vertex.substr(0, vertex.find(".vert"));
            } else if (line.find(".geom") != std::string::npos) {
                geometry = line;
            } else if (line.find(".frag") != std::string::npos) {
                fragment = line;
            } else if (line.empty() && !vertex.empty() && !fragment.empty() && !name.empty()) {
                if (shadersByName.find(name) != shadersByName.end()) {
                    std::cerr << "ERROR: Duplicate shader names, can not initialize (" << name << ")\n";
                    return false;
                }

                std::unique_ptr<Shader> shader(new Shader());

                bool attached = shader->attachShader(GL_VERTEX_SHADER, shaderPath + vertex);
                attached = attached && shader->attachShader(GL_FRAGMENT_SHADER, shaderPath + fragment);
                if (!geometry.empty()) {
                    attached = attached && shader->attachShader(GL_GEOMETRY_SHADER, shaderPath + geometry);
                }

                if (!attached) {
                    return false;
                }
                if (!shader->linkProgram()) {
                    std::cerr << "WARNING: Failed to link shader program: " << name << "\n";
                    return false;
                }
                std::cout << "Created shader: " << name << "\n";
                shadersByName.insert(std::make_pair(name, shader.get()));
                shaders.push_back(std::move(shader));
                vertex.clear();
                fragment.clear();
                geometry.clear();
                name.clear();
            } else {
                std::cerr << "ERROR: Failed to parse shaders file\n";
                return false;
            }
        }
        shaderInfo.close();
    } else {
        std::cerr << "ERROR: Could not open shader info file: " << path << "\n";
        return false;
    }

    auto addDepthMapShader = [this] (const std::string& name, Light::Type type) {
        auto found = shadersByName.find(name);
        if (found != shadersByName.end()) {
            depthMapShadersByType.insert(std::make_pair(type, found->second));
        } else {
            std::cerr << "WARNING: Depth map shader for light type " << type << " not found\n";
        }
    };

    addDepthMapShader("depthmap_point", Light::POINT);
    addDepthMapShader("depthmap_dir", Light::DIRECTIONAL);
    return true;
}

void ResourceManager::clear()
{
    textures.clear();
    cubeTextures.clear();
    materials.clear();
    for (auto it = models.begin(); it != models.end(); ) {
        if (it->first != "lowpoly_sphere.obj") {
            it = models.erase(it);
        } else {
            ++it;
        }
    }
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

const Shader* ResourceManager::getShaderByName(const std::string& name) const
{
    auto found = shadersByName.find(name);
    if (found != shadersByName.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find shader: " << name << "\n";
        return nullptr;
    }
}

GLuint ResourceManager::getShaderProgramByName(const std::string& name) const
{
    auto found = shadersByName.find(name);
    if (found != shadersByName.end()) {
        return found->second->getProgram();
    } else {
        std::cerr << "ERROR: Could not find shader: " << name << "\n";
        return 0;
    }
}

const Shader* ResourceManager::getForwardLightShader(int shaderType, Light::Type light)
{
    auto getShaderPtr = [&] () -> Shader* {
        auto found = forwardLightShadersByType.find(std::make_pair(shaderType, light));
        if (found != forwardLightShadersByType.end()) {
            return found->second;
        } else {
            return nullptr;
        }
    };

    Shader* s = getShaderPtr();
    if (s != nullptr) {
        return s;
    } else {
        if (loadForwardLightShader(shaderType)) {
            s = getShaderPtr();
            if (s != nullptr) {
                return s;
            } else {
                std::cerr << "ERROR: Could not find forward shader " << shaderType << " with light type " << light << "\n";
            }
        } else {
            std::cerr << "ERROR: Could not load forward shader " << shaderType << " with light type " << light << "\n";
        }
    }
    return nullptr;
}

const Shader* ResourceManager::getDeferredLightShader(Light::Type light)
{
    auto getShaderPtr = [&] () -> Shader* {
        auto found = deferredLightShadersByType.find(light);
        if (found != deferredLightShadersByType.end()) {
            return found->second;
        } else {
            return nullptr;
        }
    };

    Shader* s = getShaderPtr();
    if (s != nullptr) {
        return s;
    } else {
        if (loadDeferredLightShader(light)) {
            s = getShaderPtr();
            if (s != nullptr) {
                return s;
            } else {
                std::cerr << "ERROR: Could not find deferred shader with light type " << light << "\n";
            }
        } else {
            std::cerr << "ERROR: Could not load deferred shader with light type " << light << "\n";
        }
    }
    return nullptr;
}

const Shader* ResourceManager::getDepthMapShader(Light::Type light) const
{
    auto found = depthMapShadersByType.find(light);
    if (found != depthMapShadersByType.end()) {
        return found->second;
    } else {
        std::cerr << "ERROR: Could not find depth map shader for light type " << light << "\n";
        return nullptr;
    }
}

const Shader* ResourceManager::getGBufferShader(int shaderType)
{
    auto getShaderPtr = [&] () -> Shader* {
        auto found = gBufferShadersByType.find(shaderType);
        if (found != gBufferShadersByType.end()) {
            return found->second;
        } else {
            return nullptr;
        }
    };

    Shader* s = getShaderPtr();
    if (s != nullptr) {
        return s;
    } else {
        if (loadGBufferShader(shaderType)) {
            s = getShaderPtr();
            if (s != nullptr) {
                return s;
            } else {
                std::cerr << "ERROR: Could not find gbuffer shader " << shaderType << "\n";
            }
        } else {
            std::cerr << "ERROR: Could not load gbuffer shader " << shaderType << "\n";
        }
    }
    return nullptr;
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

std::string ResourceManager::getLevelPath() const
{
    return levelPath;
}

void ResourceManager::checkMissingTextures() const
{
    for (const auto& mat : materials) {
        mat.second->checkMissingTextures();
    }
}

bool ResourceManager::loadForwardLightShader(int shaderType)
{
    std::vector<ForwardLightKey> keys;
    for (int i = 0; i < Light::NUM_TYPES; ++i) {
        keys.push_back(std::make_pair(shaderType, Light::Type(i)));
    }

    auto keyFound = [&] (ForwardLightKey key) { return forwardLightShadersByType.count(key) > 0; };
    if (std::all_of(keys.begin(), keys.end(), keyFound)) {
        return true;
    }

    std::stringstream ss;
    for (const auto& tm : TEXTURE_TYPE_MAPPINGS) {
        if (shaderType & tm.shaderType) {
            ss << tm.shaderDefine;
        }
    }

    std::string path = shaderPath + FORWARD_LIGHT_SHADER;
    auto createForwardLightShader = [&] (Light::Type lightType, std::string defines)
    {
        defines += LIGHT_DEFINE_MAPPINGS.at(lightType);
        std::unique_ptr<Shader> shader(new Shader());
        bool attached = shader->attachShader(GL_VERTEX_SHADER, path + ".vert", defines);
        attached = attached && shader->attachShader(GL_FRAGMENT_SHADER, path + ".frag", defines);

        if (!attached) {
            return false;
        }
        if (!shader->linkProgram()) {
            std::cerr << "WARNING: Failed to link forward light shader for light type " << lightType << " with mask: " << std::bitset<8>(shaderType) << "\n";
            return false;
        }

        std::cout << "Created forward light shader for light type " << lightType << " with mask " << std::bitset<8>(shaderType) << "\n";
        ForwardLightKey key = std::make_pair(shaderType, lightType);
        forwardLightShadersByType.insert(std::make_pair(key, shader.get()));
        shaders.push_back(std::move(shader));
        return true;
    };

    for (int i = 0; i < Light::NUM_TYPES; ++i) {
        if (!createForwardLightShader(Light::Type(i), ss.str())) {
            return false;
        }
    }
    return true;
}

bool ResourceManager::loadDeferredLightShader(Light::Type light)
{
    if (deferredLightShadersByType.count(light) > 0) {
        return true;
    }

    std::string defines = light == Light::Type::POINT ? POINT_DEFINE : DIRECTIONAL_DEFINE;
    std::string path = shaderPath + DEFERRED_LIGHT_SHADER;
    std::unique_ptr<Shader> shader(new Shader());
    bool attached = shader->attachShader(GL_VERTEX_SHADER, path + ".vert", defines);
    attached = attached && shader->attachShader(GL_FRAGMENT_SHADER, path + ".frag", defines);

    if (!attached) {
        return false;
    }
    if (!shader->linkProgram()) {
        std::cerr << "WARNING: Failed to link deferred light shader with light: " << light << "\n";
        return false;
    }

    std::cout << "Created deferred light shader with light: " << light << "\n";
    deferredLightShadersByType.insert(std::make_pair(light, shader.get()));
    shaders.push_back(std::move(shader));
    return true;
}

bool ResourceManager::loadGBufferShader(int shaderType)
{
    if (gBufferShadersByType.count(shaderType) > 0) {
        return true;
    }

    std::stringstream ss;
    for (const auto& tm : TEXTURE_TYPE_MAPPINGS) {
        if (shaderType & tm.shaderType) {
            ss << tm.shaderDefine;
        }
    }
    std::string defines = ss.str();
    std::string path = shaderPath + GBUFFER_SHADER;
    std::unique_ptr<Shader> shader(new Shader());
    bool attached = shader->attachShader(GL_VERTEX_SHADER, path + ".vert", defines);
    attached = attached && shader->attachShader(GL_FRAGMENT_SHADER, path + ".frag", defines);

    if (!attached) {
        return false;
    }
    if (!shader->linkProgram()) {
        std::cerr << "WARNING: Failed to link gbuffer shader with mask: " << std::bitset<8>(shaderType) << "\n";
        return false;
    }

    std::cout << "Created gbuffer shader with mask " << std::bitset<8>(shaderType) << "\n";
    gBufferShadersByType.insert(std::make_pair(shaderType, shader.get()));
    shaders.push_back(std::move(shader));
    return true;
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

            aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];
            if (aMaterial) {
                std::unique_ptr<Material> mat(new Material());
                if (loadMaterial(aMaterial, mat.get())) {
                    mesh->setMaterial(mat.get());
                    auto iter = materials.insert(std::make_pair(mat->getId(), std::move(mat)));
                    if (!iter.second) {
                        std::cerr << "ERROR: Could not insert material\n";
                    }
                } else {
                    std::cerr << "WARNING: Could not load material for " << file << "\n";
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
    int shaderType = 0;
    auto loadTexture = [&] (aiTextureType aType, Material::TextureType textureType, Shader::Type currentShaderType)
    {
        if (aMaterial->GetTextureCount(aType) == 0) {
            return true;
        }

        aiString path;
        aMaterial->GetTexture(aType, 0, &path);
        GLuint tex = getTexture(std::string(path.C_Str()));
        if (tex == 0) {
            return false;
        }
        material->setTexture(tex, textureType, GL_TEXTURE_2D);
        shaderType |= currentShaderType;
        return true;
    };

    bool valid = true;
    for (const auto& tm : TEXTURE_TYPE_MAPPINGS) {
        if (!loadTexture(tm.aiType, tm.materialType, tm.shaderType)) {
            valid = false;
            break;
        }
    }

    if (!valid || !loadForwardLightShader(shaderType) || !loadGBufferShader(shaderType)) {
        return false;
    }
    material->setShaderType(shaderType);
    return true;
}

} // moar
