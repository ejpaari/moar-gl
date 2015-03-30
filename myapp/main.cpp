#include "myapp.h"
#include "../engine/engine.h"

#include <memory>

int main(/*int argc, char* argv[]*/)
{
    moar::Engine engine("../moar-gl/myapp/settings.ini");
    if (!engine.init()) {
        return -1;
    }
    std::shared_ptr<MyApp> app(new MyApp());
    engine.setApplication(app);
    engine.execute();
    return 0;
}
