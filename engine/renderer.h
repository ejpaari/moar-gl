#ifndef RENDERER_H
#define RENDERER_H

#include "component.h"
#include "model.h"

#include <glm/glm.hpp>
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
    Model* model;
    bool shadowCaster;
    bool shadowReceiver;
};

} // moar

#endif // RENDERER_H
