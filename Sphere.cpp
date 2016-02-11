#include "Include/glm/gtx/transform.hpp"
#include "Include/glm/gtc/type_ptr.hpp"
#include <algorithm>

#include "Sphere.h"
#include "spdlog/include/spdlog/spdlog.h"

Sphere::Sphere(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader):
GraphicObject(x, y, z, size, vertexShader, fragmentShader)
{

  shader_->load();

  const int polyCount = 50;
  vertices_ = generateVertices(polyCount);

  colors_ = std::vector<float>();
  for (int i = 0; i < polyCount; ++i)
  {
    colors_.push_back(0.5);
    colors_.push_back(0.5);
    colors_.push_back(0.5);

    colors_.push_back(0.5);
    colors_.push_back(0.5);
    colors_.push_back(0.5);

    colors_.push_back(0.1);
    colors_.push_back(0.1);
    colors_.push_back(0.1);
  }

  std::transform(vertices_.begin(), vertices_.end(), vertices_.begin(),
  std::bind1st(std::multiplies<float>(), size/2));

  load();
}

Sphere::Sphere(int x, int y, int z, float size):
Sphere(x, y, z, size, "../Shaders/couleur3D.vert", "../Shaders/couleur3D.frag")
{

}

void Sphere::load()
{
  //VBO
  if (glIsBuffer(VBOId_))
  {
    glDeleteBuffers(1, &VBOId_);
  }

  glGenBuffers(1, &VBOId_);
  glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

  glBufferData(GL_ARRAY_BUFFER, nbVerticesBytes() + nbColorsBytes(), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, nbVerticesBytes(), vertices_.data());
  glBufferSubData(GL_ARRAY_BUFFER, nbVerticesBytes(), nbColorsBytes(), colors_.data());

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //VAO
  if (glIsVertexArray(VAOId_))
  glDeleteVertexArrays(1, &VAOId_);

  glGenVertexArrays(1, &VAOId_);
  glBindVertexArray(VAOId_);

  glBindBuffer(GL_ARRAY_BUFFER, VBOId_);
  //Vertices
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  //Colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(nbVerticesBytes()));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

void Sphere::draw(glm::mat4 &projection, glm::mat4 &modelview)
{
  modelview = glm::translate(modelview, position_);
  glUseProgram(shader_->programID());

  glBindVertexArray(VBOId_);

  glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
  glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

  glBindVertexArray(0);

  glUseProgram(0);

  modelview = glm::translate(modelview, - position_);
}

Sphere::~Sphere()
{

}


std::vector<float> Sphere::generateVertices(int polyCount) const
{
  const float PI = 3.1416;
  float thetaRad = 2 * PI / polyCount;
  // float phiRad = thetaRad;

  std::vector<float> res;

  for (int i = 0; i < polyCount; ++i)
  {
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    z1 = z2 = x3 = y3 = 0;
    z3 = size_;

    if (i == 0)
    {
      x1 = size_;
      y1 = 0;
      x2 = size_ * cos(thetaRad);
      y2 = size_ * sin(thetaRad);
    }
    else
    {
      const int x1Index = i * 9 - 2 * 3;
      x1 = res[x1Index];
      y1 = res[x1Index + 1];
      x2 = size_ * cos((i+1) * thetaRad);
      y2 = size_ * sin((i+1) * thetaRad);
    }

    res.push_back(x1);
    res.push_back(y1);
    res.push_back(z1);

    res.push_back(x2);
    res.push_back(y2);
    res.push_back(z2);

    spdlog::get("console")->debug() << i << ": " << x1 << " " << y1 << " " << z1 << " | "
    << x2 << " " << y2 << " " << z2 << " | "
    << x3 << " " << y3 << " " << z3;

    res.push_back(x3);
    res.push_back(y3);
    res.push_back(z3);
  }

  return res;
}
