#include "Log.h"
#include "Scene.h"

int main(int argc, char** argv)
{
    bool fullscreen = false;
    bool oculusRender = false;

    logger->info(logger->get() << "Command line arguments: " << argc-1);

    for(int i=1; i<argc; i++)
    {
        std::string arg(argv[i]);

        fullscreen = (arg == "-f");
        oculusRender = (arg == "-o");
    }

    Scene scene("Simulation", WINDOW_WIDTH, WINDOW_HEIGHT, oculusRender, fullscreen);
    scene.mainLoop();
    return 0;
}
