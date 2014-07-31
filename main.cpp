#include "Log.h"
#include "Scene.h"

int main(int argc, char** argv)
{
    bool fullscreen = false;
    bool oculusRender = false;
    bool help = false;

    logger->info(logger->get() << "Command line arguments: " << argc-1);

    for(int i=1; i<argc; i++)
    {
        std::string arg(argv[i]);

        fullscreen = (arg == "-f" || arg == "--fullscreen");
        oculusRender = (arg == "-o" || arg == "--oculus");
        help = (arg == "-h" || arg == "--help");
    }

    if(help)
    {
        cout << "Usage: " << endl;
        cout << "\t-o, --oculus: Oculus mode" << endl;
        cout << "\t-f, --fullscreen: Full screen mode" << endl;
        cout << "\t-h, --help: This help message" << endl;
        return 0;
    }

    Scene scene("Simulation", WINDOW_WIDTH, WINDOW_HEIGHT, oculusRender, fullscreen);
    scene.mainLoop();
    return 0;
}
