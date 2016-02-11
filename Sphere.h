#ifndef SPHERE_H
#define SPHERE_H

#include "GraphicObject.h"

class Sphere: public GraphicObject
{
public:
  Sphere(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader);
  Sphere(int x, int y, int z, float size);
  virtual ~Sphere();
  void draw(glm::mat4 &projection, glm::mat4 &modelview);
  void load();

  /**
  * @brief Gives the memory size of the texture coordinates in bytes
  * @return The memory size of the texture coordinates in bytes
  */
  int nbTextureBytes();

  void print();

private:
  std::vector<float> generateVertices(int polyCount) const;

};

#endif
