#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <vector>

namespace moar
{

class Shader
{
public:
    explicit Shader();
    ~Shader();
    Shader(const Shader& rhs) = delete;
    Shader(Shader&& rhs) = delete;
    Shader& operator=(Shader rhs) = delete;
    Shader& operator=(Shader&& rhs) = delete;

    bool attachShader(GLenum shaderType, const char *filename);
    bool linkProgram();
    void useProgram();

    GLuint getProgram();

private:
    bool compileShader(GLuint shader, const char* filename);

    GLuint program;
    std::vector<GLuint> shaders;
};

} // moar

#endif // SHADER_H
