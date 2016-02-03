#include "spdlog/include/spdlog/spdlog.h"
#include "Scene.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

#define SPDLOG_NO_THREAD_ID
#define SPDLOG_NO_REGISTRY_MUTEX

int main(int argc, char** argv)
{
  try {
    spdlog::set_level(spdlog::level::info);
    auto console = spdlog::stdout_logger_mt("console");

    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "Produce help message")
    ("verbose,v", "Verbose logs")
    ("oculus,o", "Oculus mode")
    ("fullscreen,f", "Fullscreen mode")
    ("texture,t", po::value<std::string>()->default_value("../Textures/photorealistic/photorealistic_marble/granit01.jpg"), "Set the texture used on the cubes")
    ("number,n", po::value<unsigned long>()->default_value(1024), "Set the number of objects seen")
    ("size,s", po::value<int>()->default_value(128), "Set the size of the data cube. Must be a power of 2")
    ("octantSize", po::value<int>()->default_value(8), "Set the size of an octant. Must be a power of 2")
    ("octantDrawnCount,d", po::value<int>()->default_value(2), "Set the number of octant drawn count. 1 to only draw the octant the camera is currently in, 2 to draw the immediate neighbors, ...")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
      std::cout << desc << std::endl;
      return 0;
    }

    if (vm.count("verbose")) spdlog::set_level(spdlog::level::debug);

    Scene scene("Simulation", WINDOW_WIDTH, WINDOW_HEIGHT,
    vm.count("oculus"),
    vm.count("fullscreen"),
    vm["texture"].as<std::string>(),
    vm["number"].as<unsigned long>(),
    vm["size"].as<int>(),
    vm["octantSize"].as<int>(),
    vm["octantDrawnCount"].as<int>()
    );
    scene.mainLoop();
  }
  catch (exception& e) {
    spdlog::get("console")->error() << e.what();
    return 1;
  }

  return 0;
}
