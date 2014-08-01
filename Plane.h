#ifndef PLANE_H
#define PLANE_H

/** @file
 * @brief Plane management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 25/07/14
 */

#include "Include/glm/glm.hpp"
#include "GraphicObject.h"
#include "Texture.h"

#include <memory>

/**
 * @brief The Plane class
 * @details A plane is a simple flat 2 dimensional surface.
 * Uses the Flyweight pattern with a shared texture pool
 */
class Plane: public GraphicObject
{
public:
    Plane(float x, float y, float z, float width, float height, float repeatWidth, float repeatHeight, std::string const & vertexShader, std::string const & fragShader, std::string const & texture);
    ~Plane();
    void draw(glm::mat4 &projection, glm::mat4 &modelview);
    virtual void load();

    int nbTextureBytes();

protected:
    std::vector<float> textureCoord_;
    std::shared_ptr<Texture> texture_;
};

#endif // PLANE_H
