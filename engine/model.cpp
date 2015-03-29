#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace moar {

Model::Model() {

}

Model::~Model() {
}

bool Model::loadModel(const std::string file) {
    Assimp::Importer importer;
    const aiScene* assimpScene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (assimpScene) {
        meshes.resize(assimpScene->mNumMeshes);
        if (assimpScene->mNumMeshes == 0) {
            std::cerr << "Warning: No mesh found in the model " << file << std::endl;
        }

        for (unsigned int i = 0; i < meshes.size(); ++i) {
            const aiMesh* assimpMesh = assimpScene->mMeshes[i];
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
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
                // To do: texture coordinates
//                if (assimpMesh->HasTextureCoords(j)) {
//                    assimpMesh->mTextureCoords[j]);
//                }
            }

            for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
                if (assimpMesh->mFaces[j].mNumIndices != 3) {
                    std::cerr << "Unable to parse model indices: " << file << std::endl;
                    return false;
                }
                indices.push_back(assimpMesh->mFaces[j].mIndices[0]);
                indices.push_back(assimpMesh->mFaces[j].mIndices[1]);
                indices.push_back(assimpMesh->mFaces[j].mIndices[2]);
            }

            meshes[i] = std::unique_ptr<Mesh>(new Mesh());
            meshes[i]->setShader(shader);
            meshes[i]->setVertices(vertices);
            meshes[i]->setIndices(indices);
            meshes[i]->setNormals(normals);
            // To do: add normals and texture coordinates to mesh
        }
        return true;
    } else {
        std::cerr << "Failed to read model: " << file << std::endl;
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return false;
    }
}

void Model::render() const {
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i]->render();
    }
}

} // moar
