#ifndef APPLICATION
#define APPLICATION

#include <GL/glew.h>
#include <SOIL.h>

namespace moar {

class Application {
public:
    explicit Application();
    virtual ~Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) &  = delete;
    Application& operator=(Application&&) & = delete;

    void virtual initialize() = 0;
    void virtual run() = 0;

    bool isRunning() const { return running; }
    void quit() { running = false; }

private:
    bool running;
};

} // moar

#endif // APPLICATION
