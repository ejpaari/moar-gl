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

std::string Shader::commonVertexShaderCode = "";
std::string Shader::commonFragmentShaderCode = "";

void Shader::loadCommonShaderCode(GLenum type, const std::string& file)
{
    std::string* code = selectCommonCode(type);
    if (!code) {
        return;
    }
    code->clear();
    std::ifstream shaderFile(file.c_str());
    if (shaderFile) {
        code->append((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
        shaderFile.close();
    } else {
        std::cerr << "WARNING: Could not open common fragment shader file: " << COMMON_FRAGMENT_FILE << "\n";
    }
}

void Shader::addCommonShaderCode(GLenum type, const std::string& addition)
{
    std::string* code = selectCommonCode(type);
    if (!code) {
        return;
    }
    code->append(addition);
}

std::string* Shader::selectCommonCode(GLenum type)
{
    if (type == GL_VERTEX_SHADER) {
        return &commonVertexShaderCode;
    }
    if (type == GL_FRAGMENT_SHADER) {
        return &commonFragmentShaderCode;
    }
    return nullptr;
}

Shader::Shader()
{
    program = glCreateProgram();
    uniforms.reset();
}

Shader::~Shader()
{
    deleteShaders();
    glDeleteProgram(program);
}

bool Shader::attachShader(GLenum shaderType, const std::string& filename, const std::string& defines)
{
    GLuint shader = glCreateShader(shaderType);
    std::string common = "";
    switch (shaderType) {
    case GL_VERTEX_SHADER: common = commonVertexShaderCode; break;
    case GL_FRAGMENT_SHADER: common = commonFragmentShaderCode; break;
    }

    if (!shader || !compileShader(shader, filename, defines, common)) {
        glDeleteShader(shader);
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
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
        glDeleteProgram(program);
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

    deleteShaders();

    return isLinked == GL_TRUE;
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
        std::cerr << "WARNING: Uniform not found. " << e.what() << "\n";
        return false;
    }
}

GLuint Shader::getShadowMapLocation(int num) const
{
    return shadowMapLocations[num];
}

void Shader::deleteShaders()
{
    for (GLuint shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }
    shaders.clear();
}

bool Shader::compileShader(GLuint shader, const std::string& filename, const std::string& defines, const std::string& common)
{
    std::string shaderCode = GLSL_VERSION + defines + common;
    std::ifstream shaderFile(filename.c_str());
    if (shaderFile) {
        shaderCode.append((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
        shaderFile.close();
    } else {
        std::cerr << "WARNING: Could not open shader file: " << filename << "\n";
        return false;
    }

    const char* shaderCodeCstr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodeCstr, NULL);

    glCompileShader(shader);
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        std::cerr << "WARNING: Shader compilation failed for file " << filename << "\n";
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
        std::string outFile = "failed_shader.out";
        std::ofstream ofs(outFile.c_str());
        if (ofs) {
            ofs << shaderCode;
            ofs.close();
            std::cerr << "Shader code written to file: " << outFile << "\n";
        }
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
    for (int index = 0; index < numActiveUniforms; ++index) {
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

    glUseProgram(program);
    for (int i = 0; i < MAX_NUM_SHADOWMAPS; ++i) {
        std::stringstream ss;
        ss << "depthTexs[" << i << "]";
        GLint location = glGetUniformLocation(program, ss.str().c_str());
        shadowMapLocations[i] = location;
    }

    return true;
}

} // moar
