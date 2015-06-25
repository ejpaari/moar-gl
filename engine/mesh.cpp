#include "mesh.h"
#include "constants.h"

namespace moar
{

Mesh::Mesh() :
    numIndices(0) {
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

void Mesh::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

} // moar
