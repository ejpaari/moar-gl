#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

namespace moar {

class Mesh {
public:
    Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) &  = delete;
    Mesh& operator=(Mesh&&) & = delete;

    void setShader(GLuint shaderProgram) { shader = shaderProgram; }
    void setVertices(const std::vector<glm::vec3>& vertices);
    void setIndices(const std::vector<unsigned int>& indices);
    void setNormals(const std::vector<glm::vec3>& normals);

    unsigned int getNumIndices() const { return numIndices; }

private:
    GLuint shader;
    GLuint VAO;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint normalBuffer;
    unsigned int numIndices;    
};

} // moar

#endif // MESH_H
