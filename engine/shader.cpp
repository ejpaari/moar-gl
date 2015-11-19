#include "shader.h"
#include "common/globals.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>

namespace moar
{

const std::string Shader::INCLUDE_DIRECTIVE = "#moar::include";

Shader::Shader()
{
    program = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

bool Shader::attachShader(GLenum shaderType, const char *filename)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader || !compileShader(shader, filename)) {
        glDeleteShader(shader);
        return false;
    }

    shaders.push_back(shader);
    glAttachShader(program, shader);
    return true;
}

bool Shader::compileShader(GLuint shader, const char* filename)
{
    std::ifstream shaderFile(filename);
    std::string line;
    std::string shaderCode;

    if (shaderFile.is_open()) {
        shaderFile.seekg(0, std::ios_base::end);
        size_t shaderSize = shaderFile.tellg();
        shaderFile.seekg(0, std::ios_base::beg);
        shaderCode.reserve(shaderSize);

        while (std::getline(shaderFile, line)) {
            if (!line.empty() && line[0] == '#' && line.find(INCLUDE_DIRECTIVE) != std::string::npos) {
                auto start = line.find_first_of('"') + 1;
                auto end = line.find_last_of('"');
                std::string includeFilename = line.substr(start, end - start);
                std::ifstream includeFile(includeFilename.c_str());
                if (includeFile.is_open()) {
                    includeFile.seekg(0, std::ios_base::end);
                    size_t includeSize = includeFile.tellg();
                    includeFile.seekg(0, std::ios_base::beg);
                    shaderCode.reserve(shaderCode.capacity() + includeSize);
                    std::stringstream buffer;
                    buffer << includeFile.rdbuf();
                    shaderCode += buffer.str() + "\n";
                    includeFile.close();
                } else {
                    std::cerr << "ERROR: Could not open include file: " << includeFilename << std::endl;
                    return false;
                }
            } else {
                shaderCode += line + "\n";
            }
        }
        shaderFile.close();
    } else {
        std::cerr << "ERROR: Could not open shader file: " << filename << std::endl;
        return false;
    }

    const char* shaderCodeCstr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodeCstr , NULL);

    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[4096];
        glGetShaderInfoLog(shader, 4096, NULL, buffer);
        fprintf(stderr, "%s: %s\n", filename, buffer);
        return false;
    }

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

    for (GLuint shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return isLinked;
}

GLuint Shader::getProgram()
{
    return program;
}

} // moar
