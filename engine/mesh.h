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
    explicit Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void setShader(GLuint shaderProgram) { shader = shaderProgram; }
    void setVertices(const std::vector<glm::vec3>& vertices);
    void setIndices(const std::vector<unsigned int>& indices);
    void setNormals(const std::vector<glm::vec3>& normals);
    void setTextureCoordinates(const std::vector<glm::vec2>& coords);

    unsigned int getNumIndices() const { return numIndices; }

    void render() const;

private:
    GLuint shader;
    GLuint VAO;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint normalBuffer;
    GLuint texBuffer;
    unsigned int numIndices;    
};

} // moar

#endif // MESH_H
