#ifndef POSTPROCESS_H
#define POSTPROCESSH

#include <GL/glew.h>
#include <string>
#include <functional>
#include <unordered_map>

namespace moar
{

class Postprocess
{
public:
    explicit Postprocess();
    explicit Postprocess(const std::string& name, GLuint shader, int priority);
    Postprocess(const Postprocess&);
    Postprocess(Postprocess&&);
    Postprocess& operator=(Postprocess&);
    Postprocess& operator=(Postprocess&&);
    void bind() const;
    void setUniform(const std::string& name, std::function<void()> func);

    std::string getName() const;
    int getPriority() const;

private:
    std::string name;
    GLuint shader;
    int priority;
    std::unordered_map<std::string, std::function<void()>> uniforms;
};

} // moar

#endif // POSTPROCESS_H
