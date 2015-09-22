#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <vector>

namespace moar
{

class Shader
{
public:

    enum Type
    {
        DIFFUSE = 0,
        SPECULAR = 1,
        NORMAL_MAP = 2,
        LAST = 3
    };

    explicit Shader();
    ~Shader();
    Shader(const Shader& rhs) = delete;
    Shader(Shader&& rhs) = delete;
    Shader& operator=(Shader rhs) = delete;
    Shader& operator=(Shader&& rhs) = delete;

    bool attachShader(GLenum shaderType, const char *filename);
    bool linkProgram();

    GLuint getProgram();

private:
    bool compileShader(GLuint shader, const char* filename);

    GLuint program;    
    std::vector<GLuint> shaders;
};

} // moar

#endif // SHADER_H
