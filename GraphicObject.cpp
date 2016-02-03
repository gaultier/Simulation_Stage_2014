#include "GraphicObject.h"
#include "spdlog/include/spdlog/spdlog.h"

#include <cstring> // TODO: remove?


GraphicObject::GraphicObject(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader):
  position_ {x, y, z},
  orientation_ {0, 0, 0},
  size_ {size},
  shader_ {std::unique_ptr<Shader>(new Shader(vertexShader, fragmentShader))},
  VBOId_ {0},
  VAOId_ {0}
  {
  }

  GraphicObject::~GraphicObject()
  {
    if (glIsBuffer(VBOId_) )
    {
      glDeleteBuffers(1, &VBOId_);
    }

    if (glIsVertexArray(VAOId_))
    {
      glDeleteVertexArrays(1, &VAOId_);
    }
  }

  int GraphicObject::nbVerticesBytes()
  {
    return vertices_.size() * sizeof(float);
  }

  int GraphicObject::nbColorsBytes()
  {
    return colors_.size() * sizeof(float);
  }

  void GraphicObject::move(glm::vec3 const & value)
  {
    position_ += value;
  }

  void GraphicObject::updateVBO(void* data, int bytesSize, int offset)
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

    void* VBOAdress = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!VBOAdress)
    {
      spdlog::get("console")->error() << "Cannot get the VBO address";

      glBindBuffer(GL_ARRAY_BUFFER, 0);

      return;
    }
    memcpy(static_cast<char*>(VBOAdress) + offset, data, bytesSize);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    VBOAdress = 0;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  NullGraphicObject::NullGraphicObject()
  {
  }

  NullGraphicObject::~NullGraphicObject()
  {
  }
