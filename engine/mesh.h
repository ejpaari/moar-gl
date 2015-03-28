#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

namespace moar {

class Mesh {
public:
    Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) &  = delete;
    Mesh& operator=(Mesh&&) & = delete;

    unsigned int getNumIndices() const { return numIndices; }

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    unsigned int numIndices;    
};

} // moar

#endif // MESH_H
