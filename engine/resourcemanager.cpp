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

const Shader* ResourceManager::getShader(const std::string& name)
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
        bool isGood = loadModel(model.get(), modelFile);
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

bool ResourceManager::loadModel(Model* model, const std::string& file)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices  |
            aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
    // Todo: This leaks memory!? Try tp update!
    const aiScene* assimpScene = importer.ReadFile(file, flags);

    if (assimpScene) {
        if (assimpScene->mNumMeshes == 0) {
            std::cerr << "WARNING: No mesh found in the model; " << file << std::endl;
            return false;
        }

        for (unsigned int i = 0; i < assimpScene->mNumMeshes; ++i) {
            const aiMesh* assimpMesh = assimpScene->mMeshes[i];
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec2> texCoords;
            std::vector<unsigned int> indices;

            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                glm::vec3 v;
                v.x = assimpMesh->mVertices[j].x;
                v.y = assimpMesh->mVertices[j].y;
                v.z = assimpMesh->mVertices[j].z;
                vertices.push_back(v);
                //checkBoundingBoxLimits(v);

                glm::vec3 n;
                n.x = assimpMesh->mNormals[j].x;
                n.y = assimpMesh->mNormals[j].y;
                n.z = assimpMesh->mNormals[j].z;
                normals.push_back(n);

                if (assimpMesh->HasTangentsAndBitangents()) {
                    glm::vec3 tan;
                    tan.x = assimpMesh->mTangents[j].x;
                    tan.y = assimpMesh->mTangents[j].y;
                    tan.z = assimpMesh->mTangents[j].z;
                    tangents.push_back(tan);
                }

                if (assimpMesh->HasTextureCoords(0)) {
                    glm::vec2 t;
                    t.x = assimpMesh->mTextureCoords[0][j].x;
                    t.y = assimpMesh->mTextureCoords[0][j].y;
                    texCoords.push_back(t);
                }
            }

            for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
                if (assimpMesh->mFaces[j].mNumIndices != 3) {
                    std::cerr << "WARNING: Unable to parse model indices; " << file << std::endl;
                    return false;
                }
                indices.push_back(assimpMesh->mFaces[j].mIndices[0]);
                indices.push_back(assimpMesh->mFaces[j].mIndices[1]);
                indices.push_back(assimpMesh->mFaces[j].mIndices[2]);
            }

            std::unique_ptr<Mesh> mesh(new Mesh());

            aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
            if (assimpMaterial) {
                std::unique_ptr<Material> mat(new Material());
                if (loadMaterial(assimpMaterial, mat.get())) {
                    mesh->setDefaultMaterial(mat.get());
                    auto iter = materials.insert(std::make_pair(mat->getId(), std::move(mat)));
                    if (!iter.second) {
                        std::cerr << "ERROR: Could not insert material.\n";
                    }
                }
            }

            mesh->setVertices(vertices);
            mesh->setIndices(indices);
            mesh->setNormals(normals);
            if (assimpMesh->HasTangentsAndBitangents()) {
                mesh->setTangents(tangents);
            }
            if (assimpMesh->HasTextureCoords(0)) {
                mesh->setTextureCoordinates(texCoords);
            }
            model->addMesh(std::move(mesh));
        }
        // Todo: These should be calculated per mesh.
        //calculateCenterPointAndRadius();
        std::cout << "Loaded model: " << file << std::endl;
        return true;
    } else {
        std::cerr << "WARNING: Failed to read model; " << file << std::endl;
        std::cerr << importer.GetErrorString() << std::endl;
        return false;
    }
}

bool ResourceManager::loadMaterial(aiMaterial* assimpMaterial, Material* material)
{
    std::string shaderType;
    aiTextureType textureType = aiTextureType_DIFFUSE;
    // Todo: Add a function for texture loading.
    for (unsigned int i = 0; i < assimpMaterial->GetTextureCount(textureType); ++i) {
        if (i > 0) {
            // Todo: Support multiple textures.
            break;
        }
        aiString texturePath;
        assimpMaterial->GetTexture(textureType, i, &texturePath);
        GLuint tex = getTexture(std::string(texturePath.C_Str()));
        if (tex == 0) {
            return false;
        }
        material->setTexture(tex, Material::DIFFUSE, GL_TEXTURE_2D);
        shaderType ="diffuse";
    }

    textureType = aiTextureType_NORMALS;
    for (unsigned int i = 0; i < assimpMaterial->GetTextureCount(textureType); ++i) {
        if (i > 0) {
            // Todo: Support multiple textures.
            break;
        }
        aiString texturePath;
        assimpMaterial->GetTexture(textureType, i, &texturePath);
        GLuint tex = getTexture(std::string(texturePath.C_Str()));
        if (tex == 0) {
            return false;
        }
        material->setTexture(tex, Material::NORMAL, GL_TEXTURE_2D);
        shaderType ="normalmap";
    }

    if (shaderType.empty()) {
        return false;
    }
    material->setShaderType(shaderType);
    return true;
}

} // moar
