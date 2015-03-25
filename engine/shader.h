#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <vector>

namespace moar {

class Shader
{
public:
    explicit Shader();
    ~Shader();
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) &  = delete;
    Shader& operator=(Shader&&) & = delete;

    bool attachShader(GLenum shaderType, const char *filename);
    void linkProgram() { glLinkProgram(program); }
    void useProgram() { glUseProgram(program); }

    GLuint getProgram() { return program; }

private:
    bool compileShader(GLuint shader, const char* filename);

    GLuint program;
    std::vector<GLuint> shaders;
};

} // moar

#endif // SHADER_H
