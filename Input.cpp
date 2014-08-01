#include "Input.h"
#include "Utils.h"
#include "Scene.h"
#include "Oculus.h"
#include "LogCpp/Log.h"

#include <iostream>

Input::Input(Scene* scene):
    isOver_(false),
    mouseX_(0),
    mouseY_(0),
    mouseXRel_(0),
    mouseYRel_(0),
    scene_ (scene),
    oculus_ {std::move(nullOculus)}
{
    for(int i=0; i< SDL_NUM_SCANCODES; i++)
    {
        keyboardKeys_[(SDL_Scancode) i] = KEY::UP;
    }

    for(int i=1; i <= 7; i++)
    {
        mouseKeys_[i] = KEY::UP;
    }
}

Input::~Input()
{
}

void Input::updateEvent()
{
    mouseXRel_ = 0;
    mouseYRel_ = 0;

    while(SDL_PollEvent(&event_))
    {
        switch(event_.type)
        {
        case SDL_KEYDOWN:
            keyboardKeys_[event_.key.keysym.scancode] = KEY::DOWN;
            logger->debug(logger->get() << "The key: " << SDL_GetScancodeName(event_.key.keysym.scancode) << " was pressed");
            break;

        case SDL_KEYUP:
            keyboardKeys_[event_.key.keysym.scancode] = KEY::UP;
            logger->debug(logger->get() << "The key: " << SDL_GetScancodeName(event_.key.keysym.scancode) << " was released");
            break;

        case SDL_MOUSEBUTTONDOWN:
            mouseKeys_[event_.button.button] = KEY::DOWN;
            break;

        case SDL_MOUSEBUTTONUP:
            mouseKeys_[event_.button.button] = KEY::UP;
            break;

        case SDL_MOUSEMOTION:
            mouseX_ = event_.motion.x;
            mouseY_ = event_.motion.y;
            mouseXRel_ = event_.motion.xrel;
            mouseYRel_ = event_.motion.yrel;
            break;

        case SDL_WINDOWEVENT:
            switch(event_.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                isOver_ = true;
                break;
            case SDL_WINDOWEVENT_RESIZED:
                int width = event_.window.data1;
                int height =  event_.window.data2;
                Utils::resizeWindow(width, height);
                scene_->setWindowWidth(width);
                scene_->setWindowHeight(height);
            }
            break;

        default:
            break;
        }
    }
}

bool Input::isOver() const
{
    return isOver_;
}

bool Input::isKeyboardKeyDown(SDL_Scancode const & key) const
{
    return keyboardKeys_.at(key);
}

bool Input::isMouseKeyDown(Uint8 key)
{
    return mouseKeys_[key];
}

bool Input::isMouseMoving() const
{
    return (mouseXRel_ != 0 || mouseYRel_ != 0);
}

int Input::mouseX() const
{
    return mouseX_;
}

int Input::mouseY() const
{
    return mouseY_;
}

int Input::mouseXRel() const
{
    return mouseXRel_;
}

int Input::mouseYRel() const
{
    return mouseYRel_;
}

void Input::showCursor(bool show) const
{
    if(show)
    {
        SDL_ShowCursor(SDL_ENABLE);
    }
    else
    {
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void Input::capturePointer(bool capture) const
{
    if(capture)
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

GenericOculus* Input::oculus() const
{
    return oculus_.get();
}

void Input::setOculus(std::unique_ptr<GenericOculus> oculus)
{
    oculus_ = std::move(oculus);
}


