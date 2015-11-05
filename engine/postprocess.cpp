#include "postprocess.h"

#include <algorithm>

namespace moar
{

Postprocess::Postprocess() :
    name(""),
    shader(0),
    priority(0)
{
}

Postprocess::Postprocess(const std::string& name, GLuint shader, int priority) :
    name(name),
    shader(shader),
    priority(priority)
{
}

Postprocess::Postprocess(const Postprocess& rhs) :
    name(rhs.name),
    shader(rhs.shader),
    priority(rhs.priority),
    uniforms(rhs.uniforms)
{
}

Postprocess::Postprocess(Postprocess&& rhs) :
    name(std::move(rhs.name)),
    shader(std::move(rhs.shader)),
    priority(std::move(rhs.priority)),
    uniforms(std::move(rhs.uniforms))
{
}

Postprocess& Postprocess::operator=(Postprocess& rhs) {
    std::swap(name, rhs.name);
    std::swap(shader, rhs.shader);
    std::swap(priority, rhs.priority);
    std::swap(uniforms, rhs.uniforms);
    return *this;
}

Postprocess& Postprocess::operator=(Postprocess&& rhs) {
    name = std::move(rhs.name);
    shader = std::move(rhs.shader);
    priority = std::move(rhs.priority);
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

std::string Postprocess::getName() const
{
    return name;
}

int Postprocess::getPriority() const
{
    return priority;
}

} // moar
