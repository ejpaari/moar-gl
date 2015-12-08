#include "application.h"
#include "engine.h"

namespace moar
{

Application::Application()
{
}

Application::~Application()
{
}

void Application::quit()
{
    running = false;
}

void Application::setEngine(Engine* engine)
{
    this->engine = engine;
}

bool Application::isRunning() const
{
    return running;
}

} // moar
