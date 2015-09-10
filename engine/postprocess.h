#ifndef POSTPROCESS_H
#define POSTPROCESSH

#include <GL/glew.h>
#include <string>
#include <functional>
#include <map>

namespace moar
{

class Postprocess
{
public:
    explicit Postprocess(const std::string& name, GLuint shader);
    ~Postprocess();
    Postprocess(const Postprocess&);
    Postprocess(Postprocess&&);
    Postprocess& operator=(Postprocess&);
    Postprocess& operator=(Postprocess&&);

    void bind() const;
    void setUniform(const std::string& name, std::function<void()> func);
    void removeUniform(const std::string& name);
    std::string getName() const;

private:
    std::string name;
    GLuint shader;
    std::map<std::string, std::function<void()>> uniforms;
};

} // moar

#endif // POSTPROCESS_H
