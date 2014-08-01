#ifndef UTILS_H
#define UTILS_H

/** @file
 * @brief Gathers utility functions
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

#include "Include/glm/glm.hpp"
#include "Include/OVR/LibOVR/Src/Kernel/OVR_Math.h"

#include <ostream>
#include <string>

/** @namespace Utils
 * Namespace gathering utility functions used throughout the program
 */
namespace Utils
{
    /**
     * @brief Resizes the OpenGL viewport when the window is resized
     * @param w The new window width
     * @param h The new window height
     */
    void resizeWindow(int w, int h);

    /**
     * @brief Retrieves all the errors from OpenGL
     */
    void GLGetError();

    /**
     * @brief Converts an angle from radians to degrees
     * @param value The angle in radians
     * @return The angles in degrees
     */
    float radToDegree(float value);

    /**
     * @brief Converts an angle from degrees to radians
     * @param value The angle in degrees
     * @return The angle in radians
     */
    float degreeToRad(float value);

    /**
     * @brief Clamps an angle to a value
     * Clamps (i.e limits) an angle to a given value which is
     * both a minimum and a maximum in absolute value
     * @param phi The angle to clamp
     * @param limit The limit we clamp the angle to
     * @return The clamped value
     */
    float clamp(float phi, float limit = 89.0f);

    /**
     * @brief Clamps a 3D vector to a value
     * Clamps (i.e limits) a 3 dimensional vector to a given
     * maximum vector and a minimum vector
     * @param vecToClamp The 3 dimensional vector to clamp
     * @param clampMin The 3 dimensional vector which is the minimum limit we clamp the vector to
     * @param clampMax The 3 dimensional vector which is the maximum limit we clamp the vector to
     */
    void clamp(glm::vec3 & vecToClamp, glm::vec3 const & clampMin, glm::vec3 const & clampMax);

    /**
     * @brief Compares two floats and tells if they are equal
     * The two floats are equal only if their difference is lower than
     * the machine epsilon, which is the lowest value existing
     * between two float values
     * @param a The first float to compare
     * @param b The second float to compare
     * @return true if the two values are equal, else false
     */
    float isEqual(float a, float b);


    /**
     * @brief Converts a 4 dimensional matrix in Oculus SDK format to a 4 dimensional matrix
     * in GLM format
     * @param mat The 4 dimensional matrix in Oculus SDK format
     * @return A 4 dimensional matrix in GLM format containing the values of the matrix we converted
     */
    glm::mat4 ovr2glmMat(OVR::Matrix4f const & mat);

    /**
     * @brief Converts a 3 dimensional vector to a pretty string
     * ready to be printed
     * @param vec The 3 dimensional vector to convert to a string
     * @return A string containing the values of the vector we converted
     */
    std::string toString(glm::vec3 const & vec);

    /**
     * @brief Converts a 4 dimensional matrix to a pretty string
     * ready to be printed
     * @param mat The 4 dimensional matrix to convert to a string
     * @return A string containing the values of the matrix we converted
     */
    std::string toString(glm::mat4 const & mat);
}

#endif // UTILS_H
