#ifndef DEF_SCENEOPENGL
#define DEF_SCENEOPENGL

/** @file
 * @brief OpenGL scene
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

#include <GL/glew.h>

#define GL3_PROTOTYPES 1
#include "Include/GL3/gl3.h"

#include "Include/glm/glm.hpp"
#include "Include/glm/gtx/transform.hpp"
#include "Include/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "SDL2/SDL.h"
#include "Include/Octree/octree.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

#include "Oculus.h"

class Input;
class Camera;
class GraphicObject;

/**
 * @brief The Scene class
 * @details The OpenGL scene. It contains the graphical objects, the camera, the input, etc.
 */
class Scene
{

public:

    Scene(std::string windowTitle, int windowWidth, int windowHeight, bool oculusRender, bool fullscreen, std::string textureName, unsigned long objectsCount, int size, int octantSize, int  octantsDrawnCount);
    ~Scene();

    /**
     * @brief The main loop of the application
     */
    void mainLoop();

    /**
     * @brief The graphical rendering
     */
    void render();

    /**
     * @brief The graphical rendering
     * @param The modelview matrix
     * @param The projection matrix
     */
    void render(glm::mat4 & MV, glm::mat4 & proj);

    SDL_Window* window() const;
    int windowWidth() const;
    void setWindowWidth(int windowWidth);
    int windowHeight() const;
    void setWindowHeight(int windowHeight);

private:
    /**
     * @brief Opens the window and creates an OpenGL context
     * @return true if successful, else false
     */
    bool initWindow();

    /**
     * @brief Inits OpenGL
     * @return true if successful, else false
     */
    bool initGL();

    /**
     * @brief Generates graphical objects at random positions
     */
    void initGObjects();

    /**
     * @brief Update the current FPS
     * @param elapsedTime The time the main loop took to render 1 frame
     */
    void updateFPS(int elapsedTime);

    /**
     * @brief Perform final actions at the end of the scene/program
     */
    void doEnd();

    /**
     * @brief Number of graphical object present in the scene
     */
    const unsigned long gObjectsCount_;

    /**
     * @brief Size of the edge of the cubic scene.
     */
    const int size_;

    /**
     * @brief Number of octants we draw
     * @details 1 means we only display the octant we are currently in, 2 means we additionnally display the immediate
     * neighbours, etc
     */
    const int octantsDrawnCount_;

    /**
     * @brief The size of an octant
     * @details An octant is a cubic part of the Octree. It contains a certain amount of its nodes.
     */
    const int octantSize_;

    /**
     * @brief The window title
     * @details It contains the FPS number and is therefore updated every frame
     */
    std::string windowTitle_;

    /**
     * @brief The window width
     */
    int windowWidth_;

    /**
     * @brief The window height
     */
    int windowHeight_;

    /**
     * @brief The SDL window
     */
    SDL_Window* window_;

    /**
     * @brief The OpenGL context
     */
    SDL_GLContext context_;

    /**
     * @brief The input manager
     */
    std::unique_ptr<Input> input_;

    /**
     * @brief The octree containing all graphical objects in the scene
     */
    Octree<std::shared_ptr<GraphicObject>> gObjects_;

    std::vector<glm::vec3> livingGObjects_;

    /**
     * @brief The camera manager
     */
    std::unique_ptr<Camera> camera_;

    /**
     * @brief Boolean showing if the window is in fullscreen mode
     */
    bool fullscreen_;

    /**
     * @brief Boolean showing if we are in Oculus rendering mode
     */
    bool oculusRender_;

    /**
     * @brief Current frame per second for the application
     */
    float fps_;

    /**
     * @brief Number of frames since the start of the application
     * @details At 60 fps, the app can run for 1000 years without overflowing this number
     */
    unsigned long long frameCount_;

    std::string textureName_;
};


#endif
