#include "mesh.h"
#include "common/globals.h"

namespace moar
{

unsigned int Mesh::idCounter = 0;

Mesh::Mesh() :
    id(++idCounter)
{
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &tangentBuffer);
    glDeleteBuffers(1, &texBuffer);
    glDeleteVertexArrays(1, &VAO);
}

Material* Mesh::getDefaultMaterial() const
{
    return material;
}

unsigned int Mesh::getId() const
{
    return id;
}

glm::vec3 Mesh::getCenterPoint() const
{
    return centerPoint;
}

float Mesh::getBoundingRadius() const
{
    return boundingRadius;
}

void Mesh::setIndices(const std::vector<unsigned int>& indices)
{
    numIndices = indices.size();
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, &indices[0], GL_STATIC_DRAW);
}

void Mesh::setVertices(const std::vector<glm::vec3>& vertices)
{
    setBufferData<glm::vec3>(vertexBuffer, vertices, VERTEX_LOCATION, 3);
}

void Mesh::setTextureCoordinates(const std::vector<glm::vec2>& coords)
{
    setBufferData<glm::vec2>(texBuffer, coords, TEX_LOCATION, 2);
}

void Mesh::setNormals(const std::vector<glm::vec3>& normals)
{
    setBufferData<glm::vec3>(normalBuffer, normals, NORMAL_LOCATION, 3);
}

void Mesh::setTangents(const std::vector<glm::vec3>& tangents)
{
    setBufferData<glm::vec3>(tangentBuffer, tangents, TANGENT_LOCATION, 3);
}

void Mesh::setDefaultMaterial(Material* material)
{
    this->material = material;
}

void Mesh::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    ++G_DRAW_COUNT;
}

void Mesh::checkBoundingBoxLimits(const glm::vec3& vert)
{
    boundingBoxMax.x = std::max(vert.x, boundingBoxMax.x);
    boundingBoxMax.y = std::max(vert.y, boundingBoxMax.y);
    boundingBoxMax.z = std::max(vert.z, boundingBoxMax.z);

    boundingBoxMin.x = std::min(vert.x, boundingBoxMin.x);
    boundingBoxMin.y = std::min(vert.y, boundingBoxMin.y);
    boundingBoxMin.z = std::min(vert.z, boundingBoxMin.z);
}

void Mesh::calculateCenterPointAndRadius()
{
    centerPoint.x = (boundingBoxMax.x + boundingBoxMin.x) / 2.0f;
    centerPoint.y = (boundingBoxMax.y + boundingBoxMin.y) / 2.0f;
    centerPoint.z = (boundingBoxMax.z + boundingBoxMin.z) / 2.0f;
    boundingRadius = std::max(glm::distance(centerPoint, boundingBoxMax), glm::distance(centerPoint, boundingBoxMax));
}

} // moar
