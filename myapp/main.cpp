#include "myapp.h"
#include "../engine/engine.h"

int main(/*int argc, char* argv[]*/)
{
    moar::Engine engine;
    if (!engine.initialize()) {
        return -1;
    }
    MyApp* app = new MyApp();
    engine.setApplication(app);
    engine.start();
    return 0;
}
