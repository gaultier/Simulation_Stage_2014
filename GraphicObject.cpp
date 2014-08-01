#include "GraphicObject.h"
#include "LogCpp/Log.h"

#include <cstring>


GraphicObject::GraphicObject(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader):
    position_ {x, y, z},
    orientation_ {0, 0, 0},
    size_ {size},
    shader_ {std::unique_ptr<Shader>(new Shader(vertexShader, fragmentShader))},
    VBOId_ {0},
    VAOId_ {0}
{
    logger->trace(logger->get() << "Graphic object constructor");
}

GraphicObject::~GraphicObject()
{
    logger->trace(logger->get() << "Graphic object destructor");
    if(glIsBuffer(VBOId_)  == GL_TRUE)
    {
        glDeleteBuffers(1, &VBOId_);
    }

    if(glIsVertexArray(VAOId_) == GL_TRUE)
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

        if(VBOAdress == nullptr)
        {
            logger->error(logger->get() << "Cannot get the VBO address");

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            return;
        }
        memcpy((char*) VBOAdress + offset, data, bytesSize);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        VBOAdress = 0;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

NullGraphicObject::NullGraphicObject()
{
    logger->trace(logger->get() << "Null graphic object constructor ");
}

NullGraphicObject::~NullGraphicObject()
{
    logger->trace(logger->get() << "Null graphic object destructor ");
}
