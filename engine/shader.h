#ifndef SHADER_H
#define SHADER_H

#include "common/globals.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <bitset>

namespace moar
{

class Shader
{
public:
    enum Type
    {
        UNDEFINED = 0,
        DIFFUSE = 1 << 0,
        SPECULAR = 1 << 1,
        NORMAL = 1 << 2,
        BUMP = 1 << 3,
        DEPTH = 1 << 4
    };

    explicit Shader();
    ~Shader();
    Shader(const Shader& rhs) = delete;
    Shader(Shader&& rhs) = delete;
    Shader& operator=(Shader rhs) = delete;
    Shader& operator=(Shader&& rhs) = delete;

    bool attachShader(GLenum shaderType, const std::string& filename, const std::string& defines = "");
    bool linkProgram();

    GLuint getProgram() const;
    bool hasUniform(GLuint location) const;

private:
    bool compileShader(GLuint shader, const std::string& filename, const std::string& defines = "");
    bool readUniformLocations();

    GLuint program;    
    std::vector<GLuint> shaders;
    std::bitset<MAX_LOCATION> uniforms;
};

} // moar

#endif // SHADER_H
