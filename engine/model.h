#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>

namespace moar
{

class Model
{
public:
    explicit Model();
    ~Model();
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    bool loadModel(const std::string file);
    void render() const;

    void setShader(GLuint shaderProgram) { shader = shaderProgram; }

private:
    GLuint shader;
    std::vector<std::unique_ptr<Mesh>> meshes;
};

} // moar

#endif // MODEL_H
