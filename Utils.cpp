#include "Utils.h"
#include "LogCpp/Log.h"

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <cmath>

namespace Utils
{
int logsCount = 0;

void resizeWindow(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLGetError()
{
    for(GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError())
    {
        std::string error("OpenGl error: ");

        switch(currError)
        {
        case GL_INVALID_ENUM:
            error += " GL_INVALID_ENUM ";
            break;
        case GL_INVALID_VALUE:
            error += " GL_INVALID_VALUE ";
            break;
        case GL_INVALID_OPERATION:
            error += " GL_INVALID_OPERATION ";
            break;
        case GL_STACK_OVERFLOW:
            error += " GL_STACK_OVERFLOW ";
            break;
        case GL_STACK_UNDERFLOW:
            error += " GL_STACK_UNDERFLOW ";
            break;
        case GL_OUT_OF_MEMORY:
            error += " GL_OUT_OF_MEMORY ";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error += " GL_INVALID_FRAMEBUFFER_OPERATION ";
            break;
        case GL_TABLE_TOO_LARGE:
            error += " GL_TABLE_TOO_LARGE ";
            break;
        default:
            error += "Unknown error";
            break;
        }
        logger->error(logger->get() << error);
    }
}

float degreeToRad(float value)
{
    return value * M_PI / 180;
}

float radToDegree(float value)
{
    return 180 * value / M_PI;
}

float clamp(float phi, float limit)
{
    float res = phi;
    if(res > limit)
    {
        res = limit;
    }
    else if(res < -limit)
    {
        res = -limit;
    }

    if(phi != res)
    {
        logger->debug(logger->get() << "Clamped the angle from " << phi << " to " << res);
    }
    else
    {
        logger->debug(logger->get() << "No need to clamp the angle");
    }

    return res;
}

float isEqual(float a, float b)
{
    return (fabs(a - b) < std::numeric_limits<double>::epsilon());
}

glm::mat4 ovr2glmMat(OVR::Matrix4f const & mat)
{
    glm::mat4 res;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            res[i][j] = mat.M[i][j];
        }
    }

    return res;
}

std::string toString(glm::vec3 const & vec)
{
    return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}

std::string toString(glm::mat4 const & mat)
{
    std::string res;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            res += std::to_string(mat[i][j]) + " ";
        }
        res += "\n";
    }

    return res;
}

void clamp(glm::vec3 & vecToClamp, glm::vec3 const & clampMin, glm::vec3 const & clampMax)
{
    glm::vec3 oldVec = vecToClamp;

    for(int i=0; i<3; i++)
    {
        if(vecToClamp[i] > clampMax[i])
        {
            vecToClamp[i] = clampMax[i];
        }
        else if(vecToClamp[i] < clampMin[i])
        {
            vecToClamp[i] = clampMin[i];
        }
    }

    if(oldVec != vecToClamp)
    {
        logger->debug(logger->get() << "Clamped the vec3 from " << Utils::toString(oldVec)
                                  << " to " << Utils::toString(vecToClamp));
    }
    else
    {
        logger->debug(logger->get() << "No need to clamp the vector");
    }
}
}

