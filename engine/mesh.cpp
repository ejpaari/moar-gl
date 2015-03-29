#include "mesh.h"

namespace moar {

Mesh::Mesh() :
    numIndices(0) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::setVertices(const std::vector<glm::vec3>& vertices) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shader, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::setIndices(const std::vector<unsigned int>& indices) {
    numIndices = indices.size();
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, &indices[0], GL_STATIC_DRAW);
}

void Mesh::setNormals(const std::vector<glm::vec3>& normals) {
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);

    GLint normalAttrib = glGetAttribLocation(shader, "normal");
    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib , 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::render() const {
    glBindVertexArray(VAO);
    glUseProgram(shader);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

} // moar
