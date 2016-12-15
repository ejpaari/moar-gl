#include "shader.h"
#include "common/globals.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace moar
{

Shader::Shader()
{
    program = glCreateProgram();
    uniforms.reset();
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

bool Shader::attachShader(GLenum shaderType, const std::string& filename, const std::string& defines)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader || !compileShader(shader, filename, defines)) {
        glDeleteShader(shader);
        std::cerr << "WARNING: Failed to attach shader " << filename << "\n";
        if (!defines.empty()) {
            std::cerr << "with additional defines:\n" << defines << "\n";
        }
        return false;
    }

    shaders.push_back(shader);
    glAttachShader(program, shader);
    return true;
}

bool Shader::linkProgram()
{
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));

        glDeleteProgram(program);;
    }

    GLuint lightBlockIndex = glGetUniformBlockIndex(program, LIGHT_BLOCK_NAME);
    if (lightBlockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, lightBlockIndex, LIGHT_BINDING_POINT);
    }

    GLuint transformationBlockIndex = glGetUniformBlockIndex(program, TRANSFORMATION_BLOCK_NAME);
    if (transformationBlockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, transformationBlockIndex, TRANSFORMATION_BINDING_POINT);
    }

    if (!readUniformLocations()) {
        return false;
    }

    for (GLuint shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return isLinked;
}

GLuint Shader::getProgram() const
{
    return program;
}

bool Shader::hasUniform(GLuint location) const
{
    try {
        return uniforms.test(location);
    } catch (std::out_of_range& e) {
        return false;
    }
}

bool Shader::compileShader(GLuint shader, const std::string& filename, const std::string& defines)
{
    std::ifstream shaderFile(filename.c_str());
    std::string shaderCode = GLSL_VERSION + defines;

    if (shaderFile.is_open()) {
        shaderFile.seekg(0, std::ios::end);
        shaderCode.reserve(shaderCode.size() + shaderFile.tellg());
        shaderFile.seekg(0, std::ios::beg);
        shaderCode.append((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
        shaderFile.close();
    } else {
        std::cerr << "ERROR: Could not open shader file: " << filename << "\n";
        return false;
    }

    const char* shaderCodeCstr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodeCstr, NULL);

    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[4096];
        glGetShaderInfoLog(shader, 4096, NULL, buffer);
        fprintf(stderr, "%s: %s\n", filename.c_str(), buffer);
        return false;
    }

    return true;
}

bool Shader::readUniformLocations()
{
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    std::vector<GLenum> properties(1, GL_LOCATION);
    std::vector<GLint> values(1, -1);
    for (int index = 0; index < numActiveUniforms; ++index)
    {
        glGetProgramResourceiv(program, GL_UNIFORM, index, properties.size(), &properties[0], values.size(), NULL, &values[0]);
        GLint uniform = values[0];
        if (uniform  != -1) {
            if (uniform  < 0 || uniform >= MAX_LOCATION) {
                std::cerr << "ERROR: Uniforma locations do not match\n";
                return false;
            }
            uniforms.set(uniform);
        }
    }
    return true;
}

} // moar
