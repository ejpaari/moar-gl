#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

namespace moar
{

class Model
{
    friend class ResourceManager;

public:
    explicit Model();
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    // Todo: Move to Mesh-class.
    glm::vec3 getCenterPoint() const;
    float getBoundingRadius() const;

    const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;

private:
    void checkBoundingBoxLimits(const glm::vec3& vert);
    void calculateCenterPointAndRadius();

    void addMesh(std::unique_ptr<Mesh> mesh);

    // Todo: Mesh without pointer.
    std::vector<std::unique_ptr<Mesh>> meshes;
    glm::vec3 boundingBoxMax;
    glm::vec3 boundingBoxMin;
    glm::vec3 centerPoint;
    float boundingRadius = 0.0f;
};

} // moar

#endif // MODEL_H
