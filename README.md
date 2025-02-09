Simulation_Stage_2014
=====================

OpenGL 3.3 visualization of stars with Oculus Rift mode.

Project developped during my internship in the Observatoire Astronomique de Strasbourg.
Tested on Linux but should work on other plateforms too with little to no changes.

This project is in no means a model to follow if you're developping an OpenGL application, but is rather a proof-of-concept and demonstration
of what you can do with the Oculus Rift.

##Screenshots
![ornament](screenshots/ornament.png)
![oculus](screenshots/oculus.png)

##Install

- Clone the repo
- `git submodule init` and `git submodule update`
- Update your graphic card drivers
- Install the Oculus SDK 0.3.2 for your plateform (the files included are for the Linux Oculus SDK).
- Install the SDL2 library
- Install the SDL2 Image library
- Install the Boost Program options library (tested with 1.55)
- Make sure you have a c++14 compliant compiler (clang or gcc work fine).
- Build:

    ```sh
    mkdir build && cd build
    cmake ..
    make
    ./Simulation
    ```

- Plug-in your Oculus Rift if you have one (tested with DK1, should work with DK2). See the options for the Oculus mode. Works with or without an Oculus Rift.
- Launch (./Simulation)


## Command line options

```
-h [ --help ]                         Produce help message
-o [ --oculus ]                       Oculus mode
-f [ --fullscreen ]                   Fullscreen mode
-t [ --texture ] arg (=Textures/photorealistic/photorealistic_marble/granit01.jpg)
                                      Set the texture used on the cubes
-n [ --number ] arg (=1024)           Set the number of objects seen
-s [ --size ] arg (=128)              Set the size of the data cube. Must be
                                      a power of 2
--octantSize arg (=8)                 Set the size of an octant. Must be a
                                      power of 2
-d [ --octantDrawnCount ] arg (=2)    Set the number of octant drawn count. 1
                                      to only draw the octant the camera is
                                      currently in, 2 to draw the immediate
                                      neighbors, ...

```

Examples:

```
    ./Simulation -t Textures/photorealistic/photorealistic_marble/granit08.jpg
   ./Simulation -t Textures/photorealistic/photorealistic_marble/granit06.jpg -n 1000
   ./Simulation
   ./Simulation -h
   ./Simulation -d 1
   ./Simulation -d 2
   ./Simulation -s 64
   ./Simulation --octantSize 4

```

##How it works

The scene is an Octree which stores the objects. Only the octant we are actually in is displayed. By tweaking the value of a parameter you can also
display the neighbour octants.

##Performance
On the test computer (16 Gb RAM, 4 Gb VRAM, Intel GTX 980) it runs at around 30 FPS constant.
The initial generation takes around 100ms for 1024 objects and is linear in the number of objects.

##Documentation
Type `doxygen` in console and it should generate the documentation following the `Doxyfile` file.


##To do
- 60 fps in oculus mode
- Compilation with/without oculus SDK
- Spheres
- Glowing effect
- HD planet textures
- Threaded random generation
- Objects from map file
- Skybox
- Shady funky shaders
