#include "Scene.h"

#include "Cube.h"
#include "Input.h"
#include "Crate.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicObject.h"
#include "Plane.h"
#include "Utils.h"
#include "LogCpp/Log.h"

#include <numeric>
#include <random>
#include <chrono>

std::unique_ptr<Logger> logger(new Logger("log.log", "log.err", true, Severity::error | Severity::info));
std::unique_ptr<NullOculus> nullOculus(new NullOculus);

Scene::Scene(std::string windowTitle, int windowWidth, int windowHeight, bool oculusRender, bool fullscreen, std::string textureName, unsigned long objectsCount, int size, int octantSize, int  octantsDrawnCount):
    gObjectsCount_ {objectsCount},
    size_ {size},
    //1 to only draw the octant the camera is in, 2 to draw the immediate neighbours, etc. Power of 2
    octantsDrawnCount_ {octantsDrawnCount},
    octantSize_  {octantSize},
    windowTitle_ {windowTitle},
    windowWidth_ {windowWidth},
    windowHeight_ {windowHeight},
    window_ {nullptr},
    gObjects_ {size_},
    fullscreen_ {fullscreen},
    oculusRender_ {oculusRender},
    fps_ {0},
    frameCount_ {0},
    textureName_ {textureName}
{
    logger->trace(logger->get() << "Scene constructor");

    input_ = std::unique_ptr<Input>(new Input(this));
    input_->showCursor(false);
    input_->capturePointer(true);

    camera_ = std::unique_ptr<Camera>(new Camera(
                                          glm::vec3(size_/2, size_/2, size_/2),
                                          glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
                                          0.5,
                                          0.2,
                                          *(input_.get()))
                                      );

    gObjects_.setEmptyValue(std::shared_ptr<NullGraphicObject>(new NullGraphicObject));

    assert(initWindow());
    logger->debug(logger->get() << "Window was initialized");

    assert(initGL());
    logger->debug(logger->get() << "OpenGL was initialized");

    if(oculusRender_)
    {
        input_->setOculus(std::unique_ptr<GenericOculus>(new Oculus<Scene>(*this)));
        logger->debug(logger->get() << "Oculus view");
    }

    initGObjects();
}

Scene::~Scene()
{
    TextureFactory::destroyTextures();

    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Scene::initWindow()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        logger->error(logger->get() << "Error opening the SDL : " << SDL_GetError());
        SDL_Quit();

        return false;
    }

    //Version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Double Buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if(fullscreen_)
    {
        flags |= SDL_WINDOW_MAXIMIZED;
    }

    window_ = SDL_CreateWindow(windowTitle_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth_, windowHeight_, flags);

    if(window_ == nullptr)
    {
        logger->error(logger->get() << "Error creating the window: " << SDL_GetError());
        SDL_Quit();

        return -1;
    }

    //Context
    context_ = SDL_GL_CreateContext(window_);

    if( ! context_)
    {
        logger->error(logger->get() << "Error creating the OpenGl context: " << SDL_GetError());
        SDL_DestroyWindow(window_);
        SDL_Quit();

        return false;
    }

    return true;
}

bool Scene::initGL()
{
    glewExperimental = GL_TRUE;
    GLenum initGLEW( glewInit() );
    if(initGLEW != GLEW_OK)
    {
        logger->error(logger->get() << "Error opening GLEW : " << glewGetErrorString(initGLEW));

        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window_);
        SDL_Quit();

        return false;
    }

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Scene::initGObjects()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<> distribution(0, size_ - 1);

    auto startGeneration = std::chrono::high_resolution_clock::now();

    for(ulong i=1; i <= gObjectsCount_; i++)
    {
        int x = distribution(generator);
        int y = distribution(generator);
        int z = distribution(generator);

        auto startCrateGeneration = std::chrono::high_resolution_clock::now();
        gObjects_(x, y, z) = std::shared_ptr<Crate>(new Crate(x, y, z, 1.0, textureName_));
        auto endCrateGeneration = std::chrono::high_resolution_clock::now();

        logger->debug(logger->get() << "Generated crate n°" << i << " at position ("
                    << x << ", " << y << ", " << z << ") in "
                    << chrono::duration_cast<std::chrono::milliseconds>(endCrateGeneration - startCrateGeneration).count() << " ms");
    }
    auto endGeneration = std::chrono::high_resolution_clock::now();
    auto generationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endGeneration - startGeneration).count();

    logger->info(logger->get() << "Summary: the generation of " << gObjectsCount_ << " graphic objects took " << generationTime << " ms");
}

void Scene::mainLoop()
{
    int fpsDesired = 60;
    unsigned int frameRate = 1000 / fpsDesired;
    Uint32 start (0);
    Uint32 end  (0);
    Uint32 elapsedTime (0);

    while( ! input_->isOver())
    {
        start = SDL_GetTicks();

        input_->updateEvent();

        if(input_->isKeyboardKeyDown(SDL_SCANCODE_ESCAPE))
            break;

        if(oculusRender_)
        {
            input_->oculus()->render();
        }
        else
        {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            render();
        }

        if(!oculusRender_)
        {
            SDL_GL_SwapWindow(window_);
        }

        SDL_GL_SwapWindow(window_);
        //Wait for FPS
        end = SDL_GetTicks();
        elapsedTime = end - start;

        updateFPS(elapsedTime);

        if(elapsedTime < frameRate)
        {
            SDL_Delay(frameRate - elapsedTime);
        }

        frameCount_++;
    }

    doEnd();
}

void Scene::doEnd()
{
    logger->info(logger->get() << "Mean fps at the end; " << fps_);
}

void Scene::render()
{
    glm::mat4 projection;
    glm::mat4 modelview;

    double ratio = static_cast<double>(windowWidth_ / windowHeight_);

    projection = glm::perspective(90.0, ratio, 0.01, 100.0);
    modelview = glm::mat4(2.0);

    Scene::render(modelview, projection);
}

void Scene::render(glm::mat4 & MV, glm::mat4 & proj)
{
    int sizeToRender = octantSize_ * octantsDrawnCount_;

    double e = std::numeric_limits<double>::epsilon();
    camera_->move(glm::vec3(sizeToRender + e, sizeToRender + e, sizeToRender + e) , glm::vec3(size_ - sizeToRender -e, size_ - sizeToRender -e, size_ - sizeToRender -e));
    camera_->lookAt(MV);

    for(int z=camera_->position().z - sizeToRender; z<camera_->position().z + sizeToRender; z++)
    {
        for(int y=camera_->position().y - sizeToRender; y<camera_->position().y + sizeToRender; y++)
        {
            for(int x=camera_->position().x - sizeToRender; x<camera_->position().x + sizeToRender; x++)
            {
                gObjects_.at(x, y, z)->draw(proj, MV);
            }
        }
    }
}

SDL_Window* Scene::window() const
{
    return window_;
}

int Scene::windowWidth() const
{
    return windowWidth_;
}

void Scene::setWindowWidth(int windowWidth)
{
    windowWidth_ = windowWidth;
}
int Scene::windowHeight() const
{
    return windowHeight_;
}

void Scene::setWindowHeight(int windowHeight)
{
    windowHeight_ = windowHeight;
}

void Scene::updateFPS(int elapsedTime)
{
    //Avoid dividing by zero
    elapsedTime == 0 ? elapsedTime = 1 : elapsedTime;
    int newFps = 1000 / elapsedTime;

    //Cumulative moving average
    fps_ = fps_ + (newFps - fps_) / (frameCount_ + 1.0);

    logger->debug(logger->get() << "Current fps: " << newFps);
    logger->debug(logger->get() << "Current cumulated mean fps: " << fps_);

    std::string newTitle = windowTitle_ + " (" + std::to_string(newFps) + "FPS)";
    SDL_SetWindowTitle(window_, newTitle.c_str());
}
