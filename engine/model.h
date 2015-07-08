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

class ResourceManager;

class Model
{
public:
    explicit Model();
    ~Model();
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    bool loadModel(const std::string& file);
    void render() const;

    glm::vec3 getCenterPoint() const;
    float getBoundingRadius() const;

private:
    void checkBoundingBoxLimits(const glm::vec3& vert);
    void calculateCenterPointAndRadius();

    std::vector<std::unique_ptr<Mesh>> meshes;
    glm::vec3 boundingBoxMax;
    glm::vec3 boundingBoxMin;
    glm::vec3 centerPoint;
    float boundingRadius;
};

} // moar

#endif // MODEL_H
