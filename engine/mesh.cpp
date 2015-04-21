#include "mesh.h"

namespace moar
{

const GLuint Mesh::INDEX_VERTEX = 1;
const GLuint Mesh::INDEX_TEX = 2;
const GLuint Mesh::INDEX_NORMAL= 3;

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
    setBufferData<glm::vec3>(vertexBuffer, vertices, INDEX_VERTEX, 3);
}

void Mesh::setTextureCoordinates(const std::vector<glm::vec2>& coords)
{
    setBufferData<glm::vec2>(texBuffer, coords, INDEX_TEX, 2);
}

void Mesh::setNormals(const std::vector<glm::vec3>& normals)
{
    setBufferData<glm::vec3>(normalBuffer, normals, INDEX_NORMAL, 3);
}

GLuint Mesh::getVAO() const
{
    return VAO;
}

GLuint Mesh::getVertexBuffer() const
{
    return vertexBuffer;
}

GLuint Mesh::getNormalBuffer() const
{
    return normalBuffer;
}

GLuint Mesh::getTexBuffer() const
{
    return texBuffer;
}

unsigned int Mesh::getNumIndices() const
{
    return numIndices;
}

void Mesh::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

} // moar
