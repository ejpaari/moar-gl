#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

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
    Model* getModel();

    virtual std::string getName() final;
    virtual Component::Type getType() final;

private:
    Model* model;
};

} // moar

#endif // RENDEROBJECT_H
