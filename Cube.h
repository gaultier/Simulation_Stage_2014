#ifndef DEF_CUBE
#define DEF_CUBE

/** @file
 * @brief Cube management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 25/07/14
 */

#include "Include/glm/glm.hpp"
#include "GraphicObject.h"

#include <string>

/**
 * @brief The Cube class
 * @details A non textured, colored Cube
 */
class Cube: public GraphicObject
{
    public:
      Cube(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader);
      Cube(int x, int y, int z, float size);
      virtual ~Cube();
      void draw(glm::mat4 &projection, glm::mat4 &modelview);

      /**
       * @brief Creates the OpenGL resources (VBO & VAO) and sends the data to the graphic card
       */
      virtual void load();

protected:

};

#endif
