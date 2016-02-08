#include "model.h"

#include <iostream>
#include <algorithm>

namespace moar
{

Model::Model()
{
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

void Model::addMesh(std::unique_ptr<Mesh> mesh)
{
    meshes.push_back(std::move(mesh));
}

} // moar
