#include "postprocess.h"

#include <algorithm>

namespace moar
{

Postprocess::Postprocess(const std::string& name, GLuint shader) :
    name(name),
    shader(shader)
{
}

Postprocess::~Postprocess()
{
}

Postprocess::Postprocess(const Postprocess& rhs) :
   shader(rhs.shader),
   uniforms(rhs.uniforms) {
}

Postprocess::Postprocess(Postprocess&& rhs) :
   shader(std::move(rhs.shader)),
   uniforms(std::move(rhs.uniforms)) {
}

Postprocess& Postprocess::operator=(Postprocess& rhs) {
   std::swap(shader, rhs.shader);
   std::swap(uniforms, rhs.uniforms);
   return *this;
}

Postprocess& Postprocess::operator=(Postprocess&& rhs) {
   shader = std::move(rhs.shader);
   uniforms = std::move(rhs.uniforms);
   return *this;
}

void Postprocess::bind() const
{
    glUseProgram(shader);
    for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
        iter->second();
    }
}

void Postprocess::setUniform(const std::string& name, std::function<void()> func)
{
    uniforms[name] = func;
}

void Postprocess::removeUniform(const std::string& name)
{
    uniforms.erase(name);
}

std::string Postprocess::getName() const
{
    return name;
}

} // moar
