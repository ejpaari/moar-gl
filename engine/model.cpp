#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <iostream>

namespace moar
{

Model::Model()
{
}

Model::~Model()
{
}

bool Model::loadModel(const std::string& file)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices  |
            aiProcess_GenSmoothNormals | aiProcess_GenUVCoords;
    const aiScene* assimpScene = importer.ReadFile(file, flags);

    if (assimpScene) {
        meshes.resize(assimpScene->mNumMeshes);
        if (assimpScene->mNumMeshes == 0) {
            std::cerr << "WARNING: No mesh found in the model; " << file << std::endl;
            return false;
        }

        for (unsigned int i = 0; i < meshes.size(); ++i) {
            const aiMesh* assimpMesh = assimpScene->mMeshes[i];
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> texCoords;
            std::vector<unsigned int> indices;

            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                glm::vec3 v;
                v.x = assimpMesh->mVertices[j].x;
                v.y = assimpMesh->mVertices[j].y;
                v.z = assimpMesh->mVertices[j].z;
                vertices.push_back(v);

                glm::vec3 n;
                n.x = assimpMesh->mNormals[j].x;
                n.y = assimpMesh->mNormals[j].y;
                n.z = assimpMesh->mNormals[j].z;
                normals.push_back(n);

                glm::vec2 t;
                if (assimpMesh->HasTextureCoords(0)) {
                    t.x = assimpMesh->mTextureCoords[0][j].x;
                    t.y = assimpMesh->mTextureCoords[0][j].y;
                }
                texCoords.push_back(t);
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

            // Todo: maybe load mesh materials?

            meshes[i] = std::unique_ptr<Mesh>(new Mesh());            
            meshes[i]->setVertices(vertices);
            meshes[i]->setIndices(indices);
            meshes[i]->setNormals(normals);
            meshes[i]->setTextureCoordinates(texCoords);
        }
        std::cout << "Loaded model: " << file << std::endl;
        return true;
    } else {
        std::cerr << "WARNING: Failed to read model; " << file << std::endl;
        std::cerr << importer.GetErrorString() << std::endl;
        return false;
    }
}

void Model::render() const
{
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i]->render();
    }
}

} // moar
