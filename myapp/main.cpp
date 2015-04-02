#include "myapp.h"
#include "../engine/engine.h"

#include <memory>

int main(/*int argc, char* argv[]*/)
{
    moar::Engine engine;
    std::shared_ptr<MyApp> app(new MyApp());
    engine.setApplication(app);
    if (!engine.init("../moar-gl/myapp/settings.ini")) {
        return -1;
    }
    engine.execute();
    return 0;
}
