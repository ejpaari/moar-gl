#ifndef MESH_H
#define MESH_H

#include "material.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace moar
{

class Mesh
{
    friend class ResourceManager;
    friend class Renderer;

public:
    explicit Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    Material* getMaterial() const;
    unsigned int getId() const;
    glm::vec3 getCenterPoint() const;
    float getBoundingRadius() const;

private:
    static unsigned int idCounter;

    void setIndices(const std::vector<unsigned int>& indices);
    void setVertices(const std::vector<glm::vec3>& vertices);
    void setTextureCoordinates(const std::vector<glm::vec2>& coords);
    void setNormals(const std::vector<glm::vec3>& normals);
    void setTangents(const std::vector<glm::vec3>& tangents);
    void setMaterial(Material* material);

    void render() const;

    void checkBoundingBoxLimits(const glm::vec3& vert);
    void calculateCenterPointAndRadius();

    template<typename T>
    void setBufferData(GLuint buffer, const std::vector<T>& data, int position, int size);

    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint tangentBuffer = 0;
    GLuint texBuffer = 0;
    unsigned int numIndices = 0;

    Material* material = nullptr;
    unsigned int id;

    glm::vec3 boundingBoxMax;
    glm::vec3 boundingBoxMin;
    glm::vec3 centerPoint;
    float boundingRadius = 0.0f;
};

template<typename T>
void Mesh::setBufferData(GLuint buffer, const std::vector<T>& data, int position, int size)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(position, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position);
}

} // moar

#endif // MESH_H
