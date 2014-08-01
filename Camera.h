#ifndef DEF_CAMERA
#define DEF_CAMERA

/** @file
 * @brief Camera management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

#include "Include/glm/glm.hpp"
#include "Input.h"

#include <memory>

/**
 * @brief The Camera class
 * @details Typical FPS camera. Orientation controlled by the mouse, position controlled by the arrow keys or the ZQSD
 * keys
 */
class Camera
{
public:

    /**
     * @brief Constructor
     * @param position The position of the camera
     * @param eyeTarget The position of the point the camera points to, i.e is looking at
     * @param verticalAxis The vertical axis we choose to use. Typically it is y, so the value of this argument would be (0, 1, 0)
     * @param sensibility The sensibility of the camera, i.e by how much the camera rotates when we move the mouse
     * @param speed The speed of the camera, i.e by how much the camera moves when we hit the corresponding keyboard keys
     */
    Camera(glm::vec3 const & position, glm::vec3 const & eyeTarget, glm::vec3 const & verticalAxis, float sensibility, float speed, Input const & input);

    /**
    * @brief Destructor
    */
    ~Camera();

    /**
     * @brief Orientates the camera depending on the mouse movements
     * @param xRel The differential x mouse position
     * @param yRel The differential y mouse position
     */
    void orientate(float xRel, float yRel);

    /**
     * @brief Moves the camera depending on the user inputs and clamps the position
     * @param clampMin The minimum clamp value
     * @param clampMax The maximum clamp value
     */
    void move(glm::vec3 const & clampMin, glm::vec3 const & clampMax);

    /**
     * @brief Makes the camera look at a given point described by the modelview matrix
     * @param modelview The modelview matrix from OpenGL
     */
    void lookAt(glm::mat4 & modelview);

    /**
     * @brief Moves the camera position depending on the user inputs
     */
    void movePosition();

    /**
     * @brief Changes the camera orientation depending on the user inputs
     */
    void moveOrientation();

    /**
     * @brief Updates the point the camera is looking at from its position and rotation
     */
    void updateEyeTarget();

    void setEyeTarget();
    void setPosition(glm::vec3 position);
    glm::vec3 position();
    float sensibility() const;
    void setSensibility(float const sensibility);
    float speed() const;
    void setSpeed(float const speed);
    glm::vec3 orientation() const;
    void setOrientation(const glm::vec3 &orientation);
    float phi() const;
    void setPhi(float phi);
    float theta() const;
    void setTheta(float theta);
    void setOrientation(float phiRad, float thetaRad);

    const Input& input() const;

protected:

    /**
     * @brief The input management
     */
    const Input& input_;

    /**
     * @brief The phi angle, also sometimes reffered as yaw
     */
    float phi_;

    /**
     * @brief The theta angle, also sometimes reffered as pitch
     */
    float theta_;

    /**
     * @brief The orientation of the camera
     */
    glm::vec3 orientation_;

    /**
     * @brief The vertical axis convention
     */
    glm::vec3 verticalAxis_;

    /**
     * @brief The lateral (i.e perpendicural) move of the camera
     */
    glm::vec3 lateralMove_;

    /**
     * @brief The position of the camera
     */
    glm::vec3 position_;

    /**
     * @brief The point the camera is looking at
     */
    glm::vec3 eyeTarget_;

    /**
     * @brief The sensibility of the camera rotation
     */
    float sensibility_;

    /**
     * @brief The speed of the camera movement
     */
    float speed_;
};

#endif
