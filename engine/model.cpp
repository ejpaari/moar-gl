#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <algorithm>

namespace moar
{

Model::Model()
{
}

bool Model::loadModel(const std::string& file)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices  |
            aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
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
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec2> texCoords;
            std::vector<unsigned int> indices;

            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                glm::vec3 v;
                v.x = assimpMesh->mVertices[j].x;
                v.y = assimpMesh->mVertices[j].y;
                v.z = assimpMesh->mVertices[j].z;
                vertices.push_back(v);
                checkBoundingBoxLimits(v);

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

            meshes[i] = std::unique_ptr<Mesh>(new Mesh());            
            meshes[i]->setVertices(vertices);
            meshes[i]->setIndices(indices);
            meshes[i]->setNormals(normals);
            if (assimpMesh->HasTangentsAndBitangents()) {
                meshes[i]->setTangents(tangents);
            }
            if (assimpMesh->HasTextureCoords(0)) {
                meshes[i]->setTextureCoordinates(texCoords);
            }
        }
        // Todo: These should be calculated per mesh.
        calculateCenterPointAndRadius();
        std::cout << "Loaded model: " << file << std::endl;
        return true;
    } else {
        std::cerr << "WARNING: Failed to read model; " << file << std::endl;
        std::cerr << importer.GetErrorString() << std::endl;
        return false;
    }
}

glm::vec3 Model::getCenterPoint() const
{
    return centerPoint;
}

float Model::getBoundingRadius() const
{
    return boundingRadius;
}

const std::vector<std::unique_ptr<Mesh> >& Model::getMeshes() const
{
    return meshes;
}

void Model::checkBoundingBoxLimits(const glm::vec3& vert)
{
    boundingBoxMax.x = std::max(vert.x, boundingBoxMax.x);
    boundingBoxMax.y = std::max(vert.y, boundingBoxMax.y);
    boundingBoxMax.z = std::max(vert.z, boundingBoxMax.z);

    boundingBoxMin.x = std::min(vert.x, boundingBoxMin.x);
    boundingBoxMin.y = std::min(vert.y, boundingBoxMin.y);
    boundingBoxMin.z = std::min(vert.z, boundingBoxMin.z);
}

void Model::calculateCenterPointAndRadius()
{
    centerPoint.x = (boundingBoxMax.x + boundingBoxMin.x) / 2.0f;
    centerPoint.y = (boundingBoxMax.y + boundingBoxMin.y) / 2.0f;
    centerPoint.z = (boundingBoxMax.z + boundingBoxMin.z) / 2.0f;
    boundingRadius = std::max(glm::distance(centerPoint, boundingBoxMax), glm::distance(centerPoint, boundingBoxMax));
}

} // moar
