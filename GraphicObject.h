#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

/** @file
 * @brief Graphic object management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 25/07/14
 */

#include "Include/glm/glm.hpp"
#include "Shader.h"
#include "Utils.h"

#include <vector>
#include <string>
#include <memory>

//GL Macro
#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

/**
 * @brief The GraphicObject class
 * @details Abstract base class for all graphic objects in the scene.
 */
class GraphicObject
{
public:
    GraphicObject(float x = 0, float y = 0, float z = 0, float size = 0, std::string const & vertexShader = "", std::string const & fragmentShader = "");

    virtual ~GraphicObject();

    /**
     * @brief Displays the graphic object
     * @param projection The OpenGL projection matrix
     * @param modelview The OpenGL modelview matrix, which is the product of the model matrix and the view matrix, in this order
     */
    virtual void draw(glm::mat4 &projection, glm::mat4 &modelview) = 0;

    /**
     * @brief Gives the memory size of the vertices coordinates in bytes
     * @details It is used to send the vertices coordinates to the graphic card with a Vertex Buffer Object, indicating to OpenGL how much
     * memory this buffer takes
     * @return The memory size of the vertices coordinates in bytes
     */
    int nbVerticesBytes();

    /**
     * @brief Gives the memory size of the colors coordinates in bytes
     * @details It is used to send the colors coordinates to the graphic card with a Vertex Buffer Object, indicating to OpenGL how much
     * memory this buffer takes
     * @return The memory size of the colors coordinates in bytes
     */
    int nbColorsBytes();

    /**
     * @brief Moves the object a given vector
     * @param value The vector to add to the object position
     */
    void move(glm::vec3 const & value);

    void updateVBO(void* data, int bytesSize, int offset);

protected:
    /**
     * @brief The position of the object
     */
    glm::vec3 position_;

    /**
     * @brief The orientation of the object
     * @details Not used as of yet
     */
    glm::vec3 orientation_;

    /**
     * @brief The size of the object
     * @details It is used to scale the object. By default its value is 1.
     */
    float size_;

    /**
     * @brief The vertices coordinates of the object
     * @details OpenGL requires that these are given in a 1D array, so they are stored this way.
     */
    std::vector<float> vertices_;

    /**
     * @brief The colors coordinates of the object
     * @details OpenGL requires that these are given in a 1D array, so they are stored this way.
     * These are ignored if the object is textured.
     */
    std::vector<float> colors_;

    /**
     * @brief The shader manager
     * @details In OpenGL > 3.0 every object is displayed and tranformed through a shader.
     */
    std::unique_ptr<Shader> shader_;

    /**
     * @brief The OpenGL id of the Vertex Buffer Object which stores the vertices coordinates in the graphic card
     */
    GLuint VBOId_;

    /**
     * @brief The OpenGL id of the Vertex Array Object which stores multiple VBOs in the graphic card
     */
    GLuint VAOId_;
};

/**
 * @brief The NullGraphicObject class
 * @details Enables the Null Object pattern for Graphic Object. It means that its methods do nothing and are harmless.
 */
class NullGraphicObject: public GraphicObject
{
public:
    NullGraphicObject();

    ~NullGraphicObject();

    void release() {}

    void draw(glm::mat4 &, glm::mat4 &) {}
};



#endif // GRAPHICOBJECT_H

