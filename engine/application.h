#ifndef APPLICATION
#define APPLICATION

#include "../engine/renderobject.h"
#include "../engine/shader.h"

#include <GL/glew.h>
#include <SOIL.h>
#include <vector>
#include <map>
#include <memory>

namespace moar {

class Application {
public:
    explicit Application();
    virtual ~Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void virtual start() = 0;
    void virtual update() = 0;
    void render();
    void quit() { running = false; }

    bool createRenderObject(const std::string& shaderName, const std::string& modelName);

    bool isRunning() const { return running; }

private:
    bool running;

    std::vector<std::shared_ptr<RenderObject>> renderObjects;
    std::map<std::string, std::unique_ptr<Shader>> shaders;
};

} // moar

#endif // APPLICATION
