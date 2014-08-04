#include "Camera.h"
#include "Utils.h"
#include "Include/glm/gtx/transform.hpp"
#include "LogCpp/Log.h"
#include "Include/glm/gtc/type_ptr.hpp"


#include <cmath>

Camera::Camera(glm::vec3 const & position, glm::vec3 const & eyeTarget, glm::vec3 const & verticalAxis, float sensibility, float speed, Input const & input):
    input_ {input},
    phi_ {0},
    theta_ {0},
    orientation_ {0},
    verticalAxis_ {verticalAxis},
    lateralMove_ {0, 0, 0},
    position_ {position},
    eyeTarget_ {eyeTarget},
    sensibility_ {sensibility},
    speed_ {speed}
{
    setEyeTarget();
}

Camera::~Camera()
{
}

void Camera::orientate(float xRel, float yRel)
{
    logger->debug(logger->get() << "Orientate: xRel = " << xRel << ", yRel = " << yRel);

    phi_ += - yRel * sensibility_;
    theta_ += - xRel * sensibility_;

    phi_ = Utils::clamp(phi_);

    float phiRad = Utils::degreeToRad(phi_);
    float thetaRad = Utils::degreeToRad(theta_);

    setOrientation(phiRad, thetaRad);

    lateralMove_ = glm::cross(verticalAxis_, orientation_);
    lateralMove_ = glm::normalize(lateralMove_);

    updateEyeTarget();
}

void Camera::setOrientation(float phiRad, float thetaRad)
{
    if(verticalAxis_.x)
    {
        orientation_.x = sin(phiRad);
        orientation_.y = cos(phiRad) * cos(thetaRad);
        orientation_.z = cos(phiRad) * sin(thetaRad);
    }
    else if(verticalAxis_.y)
    {
        orientation_.x = cos(phiRad) * sin(thetaRad);
        orientation_.y = sin(phiRad);
        orientation_.z = cos(phiRad) * cos(thetaRad);
    }
    else if(verticalAxis_.z)
    {
        orientation_.x = cos(phiRad) * cos(thetaRad);
        orientation_.y = cos(phiRad) * sin(thetaRad);
        orientation_.z = sin(phiRad);
    }
}
const Input& Camera::input() const
{
    return input_;
}

void Camera::move(glm::vec3 const & clampMin, glm::vec3 const & clampMax)
{
    movePosition();
    Utils::clamp(position_, clampMin, clampMax);
    moveOrientation();

    updateEyeTarget();
}

void Camera::moveOrientation()
{
    glm::vec3 oldOrientation = orientation_;

    if(input_.isMouseMoving())
    {
        orientate(input_.mouseXRel(), input_.mouseYRel());
    }
    if(input_.oculus()->isMoving())
    {
        if(input_.oculus()->isUsingDebugHmd())
        {
            float x = input_.mouseXRel();
            float y = input_.mouseYRel();
            logger->debug(logger->get() << "Oculus is moving (debug): x = " << x << ", y = " << y);

            orientate(x, y);
        }
        else
        {
            logger->debug(logger->get() << "dAngle x: " << input_.oculus()->dAngles().x);
            float xRad = Utils::radToDegree(input_.oculus()->dAngles().x);
            float yRad = Utils::radToDegree(input_.oculus()->dAngles().y);
            logger->debug(logger->get() << "Oculus is moving (real): xRad = " << xRad << ", yRad = " << yRad);

            orientate( - xRad, - yRad);
        }
    }

    if(oldOrientation != orientation_)
    {
        logger->debug(logger->get() << "Move camera orientation from "
                    << Utils::toString(oldOrientation) << " to " << Utils::toString(orientation_));
    }
}

void Camera::movePosition()
{
    glm::vec3 oldPosition = position_;

    if(input_.isKeyboardKeyDown(SDL_SCANCODE_UP) || input_.isKeyboardKeyDown(SDL_SCANCODE_Z))
    {
        position_ += orientation_ * speed_;
    }
    if(input_.isKeyboardKeyDown(SDL_SCANCODE_DOWN) || input_.isKeyboardKeyDown(SDL_SCANCODE_S))
    {
        position_ += - orientation_ * speed_;
    }
    if(input_.isKeyboardKeyDown(SDL_SCANCODE_LEFT) || input_.isKeyboardKeyDown(SDL_SCANCODE_Q))
    {
        position_ += lateralMove_ * speed_;
    }
    if(input_.isKeyboardKeyDown(SDL_SCANCODE_RIGHT) || input_.isKeyboardKeyDown(SDL_SCANCODE_D))
    {
        position_ += - lateralMove_ * speed_;
    }

    if(oldPosition != position_)
    {

        logger->debug(logger->get() << "Move camera position from "
                    << Utils::toString(oldPosition) << " to " << Utils::toString(position_));
    }
}

void Camera::lookAt(glm::mat4 & modelview)
{
    modelview = glm::lookAt(position_, eyeTarget_, verticalAxis_);

    logger->debug(logger->get() << "Camera look at " << Utils::toString(modelview));
}

void Camera::setEyeTarget()
{
    orientation_ = eyeTarget_ - position_;
    orientation_ = glm::normalize(orientation_);

    if(verticalAxis_.x)
    {
        phi_ = asin(orientation_.x);
        theta_ = acos(orientation_.y / cos(phi_));

        if(orientation_.y < 0)
        {
            theta_ *= -1;
        }
    }
    else if(verticalAxis_.y)
    {
        phi_ = asin(orientation_.y);
        theta_ = acos(orientation_.z / cos(phi_));

        if(orientation_.z < 0)
        {
            theta_ *= -1;
        }
    }
    else
    {
        phi_ = asin(orientation_.x);
        theta_ = acos(orientation_.z / cos(phi_));

        if(orientation_.z < 0)
        {
            theta_ *= -1;
        }
    }

    phi_ = Utils::radToDegree(phi_);
    theta_ = Utils::radToDegree(theta_);

    logger->debug(logger->get() << "Camera set eye target: phi = " << phi_ << ", theta = " << theta_);
}

void Camera::setPosition(glm::vec3 position)
{
    position_ = position;

    updateEyeTarget();
}

glm::vec3 Camera::position()
{
    return position_;
}

float Camera::sensibility() const
{
    return sensibility_;
}

float Camera::speed() const
{
    return speed_;
}

void Camera::setSensibility(float const sensibility)
{
    sensibility_ = sensibility;
}

void Camera::setSpeed(float const speed)
{
    speed_ = speed;
}

glm::vec3 Camera::orientation() const
{
    return orientation_;
}

void Camera::setOrientation(const glm::vec3 &orientation)
{
    orientation_ = orientation;
    updateEyeTarget();
}

void Camera::updateEyeTarget()
{
    eyeTarget_ = position_ + orientation_;

    logger->debug(logger->get() << "Update eye target: " << Utils::toString(eyeTarget_));
}
float Camera::phi() const
{
    return phi_;
}

void Camera::setPhi(float phi)
{
    phi_ = phi;
}
float Camera::theta() const
{
    return theta_;
}

void Camera::setTheta(float theta)
{
    theta_ = theta;
}


