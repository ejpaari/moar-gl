#ifndef RENDERER_H
#define RENDERER_H

#include "component.h"
#include "model.h"

#include <string>

namespace moar
{

class Renderer : public Component
{
public:
    explicit Renderer();
    virtual ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    virtual void execute() final;

    void setModel(Model* model);
    void setShadowCaster(bool caster);
    void setShadowReceiver(bool receiver);

    Model* getModel();
    bool isShadowCaster() const;

    virtual std::string getName() final;
    virtual Component::Type getType() final;

private:
    Model* model = nullptr;
    bool shadowCaster = true;
    bool shadowReceiver = true;
};

} // moar

#endif // RENDERER_H
