#include "myapp.h"
#include "../engine/engine.h"

#include <memory>
#include <cstdlib>

int main(/*int argc, char* argv[]*/)
{
    moar::Engine engine;
    MyApp* app = new MyApp();
    engine.setApplication(app);
    if (!engine.init("../moar-gl/myapp/settings.ini")) {
        return EXIT_FAILURE;
    }
    engine.execute();
    return EXIT_SUCCESS;
}
