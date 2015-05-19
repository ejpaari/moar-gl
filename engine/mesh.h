#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace moar
{

class Mesh
{
public:
    static const GLuint INDEX_VERTEX;
    static const GLuint INDEX_TEX;
    static const GLuint INDEX_NORMAL;

    explicit Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void setIndices(const std::vector<unsigned int>& indices);
    void setVertices(const std::vector<glm::vec3>& vertices);
    void setTextureCoordinates(const std::vector<glm::vec2>& coords);
    void setNormals(const std::vector<glm::vec3>& normals);

    void render() const;
    // Todo: AOS vs. SOA for better cache

private:
    template<typename T>
    void setBufferData(GLuint buffer, const std::vector<T>& data, int position, int size);

    GLuint VAO;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint normalBuffer;
    GLuint texBuffer;
    unsigned int numIndices;    
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
