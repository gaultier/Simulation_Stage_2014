#ifndef OCULUS_H
#define OCULUS_H

/** @file
 * @brief All Oculus related features live in here
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

#include <GL/glew.h>

#include "Include/OVR/LibOVR/Include/OVR.h"
#include "Include/OVR/LibOVR/Src/OVR_CAPI.h"
#include "Include/OVR/LibOVR/Src/OVR_CAPI_GL.h"
#include "Include/OVR/LibOVR/Src/Kernel/OVR_Math.h"
#include "SDL2/SDL.h"
#define GL3_PROTOTYPES 1
#include "Include/GL3/gl3.h"
#include "Include/glm/glm.hpp"
#include "SDL2/SDL_syswm.h"
#include "Utils.h"
#include "LogCpp/Log.h"

#include <iostream>
//To ignore the asserts uncomment this line:
//#define NDEBUG
#include <cassert>
#include <cmath>
#include <limits>
#include <string>
#include <memory>

/**
 * @brief The GenericOculus class
 */
class GenericOculus
{
public:
    virtual ~GenericOculus() {}
    virtual void render() = 0;

    virtual void getInput() {}

    virtual bool isMoving() const;

    virtual bool isUsingDebugHmd();

    virtual glm::vec3 dAngles() const;
};


template<class T>
/**
 * @brief The Oculus templated class
 * @details It is a singleton to avoid initializing/releasing the Oculus SDK multiple times.
 * The template argument is the type of the OpeGL scene we render.
 */
class Oculus: public GenericOculus
{
public:
    /**
     * @brief Constructor
     * @details Initializes the Oculus SDK, creates a debug Oculus Rift if none is connected, and starts the sensors.
     * @param scene The OpenGL scene that contains the objects render
     */
    Oculus(T & scene):
        scene_ {scene},
        textureId_ {0},
        FBOId_ {0},
        depthBufferId_ {0},
        hmd_ {0},
        windowSize_ {0, 0},
        textureSizeLeft_ {0, 0},
        textureSizeRight_ {0, 0},
        textureSize_ {0, 0},
        angles_ {0, 0, 0},
        dAngles_ {0, 0, 0},
        distortionCaps_ {0},
        usingDebugHmd_ {false},
        multisampleEnabled_ {false}
    {
        //Oculus is a singleton and cannot be instanciated twice
        assert(!alreadyCreated);
        logger->debug(logger->get() << "Oculus constructor" );

        ovr_Initialize();

        hmd_ = ovrHmd_Create(0);

        if(!hmd_)
        {
            hmd_ = ovrHmd_CreateDebug(ovrHmd_DK1);
            usingDebugHmd_ = true;

            //Cannot create the debug hmd
            assert(hmd_);

            logger->debug(logger->get() << "Using the debug hmd");
        }

        ovrHmd_GetDesc(hmd_, &hmdDesc_);

        computeSizes();

        distortionCaps_ = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

        eyeFov_[0] = hmdDesc_.DefaultEyeFov[0];
        eyeFov_[1] = hmdDesc_.DefaultEyeFov[1];

        setOpenGLState();
        initFBO();
        initTexture();
        initDepthBuffer();

        computeSizes();
        setCfg();
        setEyeTexture();
        ovrBool configurationRes = ovrHmd_ConfigureRendering(hmd_, &cfg_.Config, distortionCaps_, eyeFov_, eyeRenderDesc_);
        //Cannot configure OVR rendering
        assert(configurationRes);

        ovrHmd_StartSensor(hmd_, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection | ovrSensorCap_Position, ovrSensorCap_Orientation);

        Oculus::alreadyCreated = true;
    }

    /**
     * @brief Destructor
     * @details Releases the Oculus SDK and the OpenGL resources required for the Oculus rendering
     */
    ~Oculus()
    {
        logger->debug(logger->get() << "Oculus destructor");
        glDeleteFramebuffers(1, &FBOId_);
        glDeleteTextures(1, &textureId_);
        glDeleteRenderbuffers(1, &depthBufferId_);

        ovrHmd_Destroy(hmd_);

        ovr_Shutdown();

        Oculus::alreadyCreated = false;
    }

    /**
     * @brief Renders the OpenGL scene with the Oculus effects
     */
    void render()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);

        frameTiming_ = ovrHmd_BeginFrame(hmd_, 0);

        // Bind the FBO...
        glBindFramebuffer(GL_FRAMEBUFFER, FBOId_);
        // Clear...
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        getInput();

        ovrPosef eyeRenderPose[2];

        for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
        {
            ovrEyeType eye = hmdDesc_.EyeRenderOrder[eyeIndex];
            eyeRenderPose[eye] = ovrHmd_BeginEyeRender(hmd_, eye);

            glViewport(eyeTexture_[eye].OGL.Header.RenderViewport.Pos.x,
                       eyeTexture_[eye].OGL.Header.RenderViewport.Pos.y,
                       eyeTexture_[eye].OGL.Header.RenderViewport.Size.w,
                       eyeTexture_[eye].OGL.Header.RenderViewport.Size.h
                       );

            // Get Projection and ModelView matrici from the device...
            OVR::Matrix4f MV = OVR::Matrix4f::Translation(eyeRenderDesc_[eye].ViewAdjust)
                    * OVR::Matrix4f(OVR::Quatf(eyeRenderPose[eye].Orientation).Inverted());

            OVR::Matrix4f Proj = OVR::Matrix4f(ovrMatrix4f_Projection(eyeRenderDesc_[eye].Fov, 0.01f, 10000.0f, true));

            glm::mat4 glmMV = Utils::ovr2glmMat(MV.Transposed());

            glm::mat4 glmProj = Utils::ovr2glmMat(Proj.Transposed());

            scene_.render(glmMV, glmProj);
            Utils::GLGetError();

            ovrHmd_EndEyeRender(hmd_, eye, eyeRenderPose[eye], &eyeTexture_[eye].Texture);
        }

        ovrHmd_EndFrame(hmd_);
    }

    /**
     * @brief Tells if we are using a debug Oculus Rift
     * @return true if no Oculus Rift is connected and we had to create a debug one, else false
     */
    bool isUsingDebugHmd()
    {
        return usingDebugHmd_;
    }

    /**
     * @brief Tells if the Oculus Rift the moving
     * @details It compares the current angular position with the previous angular position
     * @return true if the Oculus Rift if moving, else false
     */
    bool isMoving() const
    {
        bool res = false;
        for(int i=0; i < 3; i++)
        {
            res = res && Utils::isEqual(angles_[i], dAngles_[i]);
        }
        return !res;
    }

    glm::vec3 angles() const
    {
        return angles_;
    }

    void setAngles(const glm::vec3 &angles)
    {
        angles_ = angles;
    }

    /**
     * @brief Retrieves the values from the Oculus Rift sensors
     * @details It gets the current angular position from the sensors and the prediction tool, and stores the old angular position.
     * @warning The angles from the sensors are in radians and OpenGL expects angles in degrees, hence the required conversion
     * @warning If no Oculus Rift is connected and we had to create a debug one, there are no values to be retrieved: We use the mouse position.
     */
    void getInput()
    {
        glm::vec3 oldAngles = angles_;

        sensorState_ = ovrHmd_GetSensorState(hmd_, frameTiming_.ScanoutMidpointSeconds);

        if(sensorState_.StatusFlags & (ovrStatus_OrientationTracked	| ovrStatus_PositionTracked))
        {
            ovrPosef pose = sensorState_.Predicted.Pose;
            OVR::Quatf quat = pose.Orientation;

            quat.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&angles_.x, &angles_.y, &angles_.z);

            dAngles_ = angles_ - oldAngles;

            logger->debug(logger->get() << "Angles: "
                        << OVR::RadToDegree(angles_[0]) << ", "
                        << OVR::RadToDegree(angles_[1]) << ", "
                        << OVR::RadToDegree(angles_[1]) << " degrees");

            logger->debug(logger->get() << "Angles: "
                        << angles_[0] << ", "
                        << angles_[1] << ", "
                        << angles_[1] << " rad");

            logger->debug(logger->get() << "DAngles: "
                        << OVR::RadToDegree(dAngles_[0]) << ", "
                        << OVR::RadToDegree(dAngles_[1]) << ", "
                        << OVR::RadToDegree(dAngles_[1]) << " degrees");
        }
        else
        {
            logger->debug(logger->get() << "No input data (using debug hmd)");
        }
    }

protected:
    /**
     * @brief Creates the OpenGL texture required for the Oculus rendering
     * @details The Oculus rendering makes under the hood a double (for each eye) render to texture of the scene and then
     * displays this texture to the screen, hence the big size of the texture.
     */
    void initTexture()
    {
        // The texture we're going to render to...
        glGenTextures(1, &textureId_);
        // "Bind" the newly created texture : all future texture functions will modify this texture...
        glBindTexture(GL_TEXTURE_2D, textureId_);
        // Give an empty image to OpenGL (the last "0")
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize_.w, textureSize_.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        // Linear filtering...
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        Utils::GLGetError();
    }

    /**
     * @brief Creates the Frame Buffer Object needed for the Oculus rendering
     * @details The Oculus rendering uses this FBO to send the texture to the graphic card
     */
    void initFBO()
    {
        // We will do some offscreen rendering, setup FBO...
        assert(textureSize_.w != 0);
        assert(textureSize_.h != 0);

        glGenFramebuffers(1, &FBOId_);
        Utils::GLGetError();
        //Cannot create the FBO
        assert(FBOId_ != 0);

        glBindFramebuffer(GL_FRAMEBUFFER, FBOId_);
        Utils::GLGetError();
    }

    /**
     * @brief Creates the depth buffer needed for the Oculus rendering
     */
    void initDepthBuffer()
    {
        glGenRenderbuffers(1, &depthBufferId_);
        Utils::GLGetError();
        //Cannot create the depth buffer
        assert(depthBufferId_ != 0);

        glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId_);
        Utils::GLGetError();

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureSize_.w, textureSize_.h);
        Utils::GLGetError();

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId_);
        Utils::GLGetError();

        // Set the texture as our colour attachment #0...
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId_, 0);
        Utils::GLGetError();

        // Set the list of draw buffers...
        GLenum GLDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

        glDrawBuffers(1, GLDrawBuffers); // "1" is the size of DrawBuffers
        Utils::GLGetError();

        // Check if everything is OK...
        GLenum check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

        //There is a problem with the FBO
        assert(check == GL_FRAMEBUFFER_COMPLETE);

        // Unbind...
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Utils::GLGetError();

    }

    /**
     * @brief Sets some OpenGL states to adequate values for the Oculus rendering
     * @warning The multisample value does not seem the be taken into account by the Oculus SDK as of yet
     *  and the Oculus rendering seems unchanged
     */
    void setOpenGLState()
    {
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        if(multisampleEnabled_)
        {
            glEnable(GL_MULTISAMPLE);
        }
    }

    /**
     * @brief Sets the Oculus SDK configuration to adequate values for the Oculus rendering
     * @warning The Windows and OSX modes have not been tested but should work just fine
     */
    void setCfg()
    {
        cfg_.OGL.Header.API = ovrRenderAPI_OpenGL;
        cfg_.OGL.Header.Multisample = multisampleEnabled_;
        cfg_.OGL.Header.RTSize.w = windowSize_.w;
        cfg_.OGL.Header.RTSize.h = windowSize_.h;

        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        SDL_bool infoRes = SDL_GetWindowWMInfo(scene_.window(), &info);
        //Cannot retrieve SDL window info
        assert(infoRes == SDL_TRUE);

        #if defined(OVR_OS_WIN32)
            cfg.OGL.Window = info.info.win.window;
        #elif defined (OVR_OS_MAC)
            cfg.OGL.Window = info.info.cocoa.window
        #elif defined(OVR_OS_LINUX)
            cfg_.OGL.Win = info.info.x11.window;
            cfg_.OGL.Disp = info.info.x11.display;
        #endif
    }

    /**
     * @brief Sets the Oculus SDK texture configuration to adequate values for the Oculus rendering
     */
    void setEyeTexture()
    {
        eyeTexture_[0].OGL.Header.API = ovrRenderAPI_OpenGL;
        eyeTexture_[0].OGL.Header.TextureSize.w = textureSize_.w;
        eyeTexture_[0].OGL.Header.TextureSize.h = textureSize_.h;
        eyeTexture_[0].OGL.Header.RenderViewport.Pos.x = 0;
        eyeTexture_[0].OGL.Header.RenderViewport.Pos.y = 0;
        eyeTexture_[0].OGL.Header.RenderViewport.Size.h = textureSize_.h;
        eyeTexture_[0].OGL.Header.RenderViewport.Size.w = textureSize_.w/2;
        eyeTexture_[0].OGL.TexId = textureId_;

        // Right eye the same, except for the x-position in the texture...
        eyeTexture_[1] = eyeTexture_[0];
        eyeTexture_[1].OGL.Header.RenderViewport.Pos.x = (textureSize_.w + 1) / 2;

    }

    /**
     * @brief Computes the texture size
     * @details This computation depends on the window dimensions. The optimal dimensions are 1280*800, which is the Oculus
     * resolution
     * @warning Other resolutions and window resizing have not been tested but should work just fine
     */
    void computeSizes()
    {
        windowSize_.w = scene_.windowWidth();
        windowSize_.h = scene_.windowHeight();

        logger->debug(logger->get() << "Fov: " << Utils::radToDegree(2 * atan(hmdDesc_.DefaultEyeFov[0].UpTan)));

        textureSizeLeft_ = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Left, hmdDesc_.DefaultEyeFov[0], 1.0f);
        textureSizeRight_ = ovrHmd_GetFovTextureSize(hmd_, ovrEye_Right, hmdDesc_.DefaultEyeFov[1], 1.0f);
        textureSize_.w = textureSizeLeft_.w + textureSizeRight_.w;
        textureSize_.h = (textureSizeLeft_.h > textureSizeRight_.h ? textureSizeLeft_.h : textureSizeRight_.h);

    }

    glm::vec3 dAngles() const
    {
        return dAngles_;
    }

    /**
     * @brief Boolean that shows whether or not an instance has already been created
     * @details Part of the Singleton Pattern
     */
    static bool alreadyCreated;

    /**
     * @brief The generic OpenGL scene
     * @details Oculus is a templated class and its only argument is the type of \a scene. The only requirement is that scene
     * has a method \a render, wich takes as argument the modelview matrix and the projection matrix.
     */
    T & scene_;

    //GL
    /**
     * @brief The id of the OpenGL texture used in the Oculus rendering
     */
    GLuint textureId_;

    /**
     * @brief The id of the OpenGL Frame Buffer Object used in the Oculus rendering
     */
    GLuint FBOId_;

    /**
     * @brief The id of the OpenGL depth buffer used in the Oculus rendering
     */
    GLuint depthBufferId_;

    //OVR
    /**
     * @brief The Oculus Rift
     * @details If no Oculus Rift is connected, a debug one is created. The last does not have proper sensors.
     */
    ovrHmd hmd_;

    /**
     * @brief The description of the Oculus Rift
     * @details Contains lots of values like inter-pupillary distance, resolution, etc
     */
    ovrHmdDesc hmdDesc_;

    /**
     * @brief The description of each eye
     * @details Contains lots of values like dimensions, wether it is the left or right eye, etc.
     */
    ovrEyeRenderDesc eyeRenderDesc_[2];

    /**
     * @brief The texture of each eye
     */
    ovrGLTexture eyeTexture_[2];

    /**
     * @brief The Field of View of each eye
     */
    ovrFovPort eyeFov_[2];

    /**
     * @brief The configuration for the OpenGL Oculus rendering
     */
    ovrGLConfig cfg_;

    /**
     * @brief The dimensions of the window
     */
    ovrSizei windowSize_;

    /**
     * @brief The dimensions of the texture that the left eye can see
     */
    ovrSizei textureSizeLeft_;

    /**
     * @brief The dimensions of the texture that the right eye can see
     */
    ovrSizei textureSizeRight_;

    /**
     * @brief The dimensions of the texture overall
     */
    ovrSizei textureSize_;

    /**
     * @brief Time variable used by the sensor and the predication tool
     */
    ovrFrameTiming frameTiming_;

    /**
     * @brief The Oculus Rift sensors
     */
    ovrSensorState sensorState_;

    /**
     * @brief The Oculus Rift angular position
     */
    glm::vec3 angles_;

    /**
     * @brief The Oculus Rift angular position variation
     */
    glm::vec3 dAngles_;

    /**
     * @brief Flag used for the Oculus rendering configuration
     */
    int distortionCaps_;

    /**
     * @brief Boolean indicating if we are using a debug Oculus Rift
     */
    bool usingDebugHmd_;

    /**
     * @brief Boolean indicating if the Oculus rendering is multisampled
     * @warning The Oculus SDK does not seem to take this variable into account as of yet
     */
    bool multisampleEnabled_;
};

template<class T>
bool Oculus<T>::alreadyCreated = false;

/**
 * @brief The NullOculus class
 * @details Part of the Null object pattern
 */
class NullOculus: public GenericOculus
{
public:
    NullOculus();

    ~NullOculus();

    void render() {}
};

/**
 * @brief nullOculus
 * @details Implements the null object pattern
 */
extern std::unique_ptr<NullOculus> nullOculus;

#endif
