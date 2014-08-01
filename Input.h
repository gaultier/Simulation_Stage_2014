#ifndef DEF_INPUT
#define DEF_INPUT

/** @file
 * @brief Input management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

#include <SDL2/SDL.h>
#include "Include/glm/glm.hpp"
#include "Oculus.h"

#include <map>
#include <memory>

class Scene;

/**
 * @brief Simple aliasing of the UP and DOWN states for a keyboard key or a mouse button to a boolean value
 */
enum KEY
{
    UP = 0,
    DOWN = 1
};

/**
 * @brief The Input class
 * @details Handles input for the mouse, the keyboard and the Oculus Rift sensors
 */
class Input
{
  public:
    /**
     * @brief Constructor
     * @param scene The OpenGL scene that the inputs have an effect on
     */
    Input(Scene* scene);

    /**
     * @brief Destructor
     */
    ~Input();

    /**
     * @brief Updates the state of all the possible events
     * @details It is called every frame
     */
    void updateEvent();

    bool isOver() const;
    bool isKeyboardKeyDown(SDL_Scancode const & key) const;
    bool isMouseKeyDown(Uint8 key);

    /**
     * @brief Tells if the mouse is moving by watching its differential position
     * @return true if the mouse if moving, else false
     */
    bool isMouseMoving() const;

    /**
     * @brief Tells if the Oculus Rift is moving by watching its differential angular position
     * @return true if the Oculus Rift if moving, else false
     */
    bool isOculusMoving() const;

    int mouseX() const;
    int mouseY() const;
    int mouseXRel() const;
    int mouseYRel() const;
    GenericOculus* oculus() const;
    void setOculus(std::unique_ptr<GenericOculus> oculus);

    /**
     * @brief Activates or deactivates the showing of the mouse cursor
     * @param show Boolean to whether show or hide the mouse cursor
     */
    void showCursor(bool show) const;

    /**
     * @brief Activates or deactivates the containing of the mouse cursor inside the window
     * @param capture Boolean to whether contain or let free the mouse cursor
     */
    void capturePointer(bool capture) const;

private:
    /**
     * @brief The last SDL event that occured
     */
    SDL_Event event_;

    /**
     * @brief Is the application terminated?
     */
    bool isOver_;

    /**
     * @brief Hashtable where the key is the keyboard key code and the value is its state (up or down)
     */
    std::map< SDL_Scancode, bool > keyboardKeys_;

    /**
     * @brief Hashtable where the key is the mouse button code and the value is its state (up or down)
     */
    std::map< Uint8, bool > mouseKeys_;

    /**
     * @brief Mouse position on the X (horizontal) axis
     */
    int mouseX_;

    /**
     * @brief Mouse position on the Y (vertical) axis
     */
    int mouseY_;

    /**
     * @brief Differential mouse position on the X (horizontal) axis
     */
    int mouseXRel_;

    /**
     * @brief Differential mouse position on the Y (vertical) axis
     */
    int mouseYRel_;

    /**
     * @brief The OpenGL scene that is affected by the user inputs
     */
    Scene* scene_;

    /**
     * @brief The Oculus Rift whose sensors give its angular position
     */
    std::unique_ptr<GenericOculus> oculus_;
};

#endif
