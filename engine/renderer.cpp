#include "renderer.h"
#include "object.h"
#include "globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace moar
{

Renderer::Renderer() :
    model(nullptr),
    shadowCaster(true),
    shadowReceiver(true)
{
}

Renderer::~Renderer()
{
}

void Renderer::execute()
{
    if (shadowReceiver) {
        glUniform1i(RECEIVE_SHADOWS_LOCATION, static_cast<int>(shadowReceiver));
    }
    model->render();
}

void Renderer::setModel(Model* model)
{
    this->model = model;
}

void Renderer::setShadowCaster(bool caster)
{
    shadowCaster = caster;
}

void Renderer::setShadowReceiver(bool receiver)
{
    shadowReceiver = receiver;
}

Model* Renderer::getModel()
{
    return model;
}

bool Renderer::isShadowCaster() const
{
    return shadowCaster;
}

std::string Renderer::getName()
{
    return "Renderer";
}

Component::Type Renderer::getType()
{
    return Component::Type::RENDERER;
}

} // moar

