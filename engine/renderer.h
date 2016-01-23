#ifndef RENDERER_H
#define RENDERER_H

#include "model.h"
#include "shader.h"

#include <string>

namespace moar
{

class Renderer
{
public:
    explicit Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void execute(const Shader* shader);

    void setModel(Model* model);
    void setShadowCaster(bool caster);
    void setShadowReceiver(bool receiver);

    Model* getModel();
    bool isShadowCaster() const;

private:
    Model* model = nullptr;
    bool shadowCaster = true;
    bool shadowReceiver = true;
};

} // moar

#endif // RENDERER_H
