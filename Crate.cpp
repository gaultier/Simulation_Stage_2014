#include "Utils.h"
#include "Crate.h"
#include "Include/glm/gtx/transform.hpp"
#include "Include/glm/gtc/type_ptr.hpp"
#include "LogCpp/Log.h"

Crate::Crate(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader, std::string const & textureFile):
    Cube(x, y, z, size, vertexShader, fragmentShader),
    texture_ {nullptr}
{
    logger->trace(logger->get() << "Crate constructor: " << textureFile);
    //Shared texture pool
    texture_ = TextureFactory::createTexture(textureFile);

    textureCoord_ = {0, 0,   1, 0,   1, 1,     // Face 1
                     0, 0,   0, 1,   1, 1,     // Face 1

                     0, 0,   1, 0,   1, 1,     // Face 2
                     0, 0,   0, 1,   1, 1,     // Face 2

                     0, 0,   1, 0,   1, 1,     // Face 3
                     0, 0,   0, 1,   1, 1,     // Face 3

                     0, 0,   1, 0,   1, 1,     // Face 4
                     0, 0,   0, 1,   1, 1,     // Face 4

                     0, 0,   1, 0,   1, 1,     // Face 5
                     0, 0,   0, 1,   1, 1,     // Face 5

                     0, 0,   1, 0,   1, 1,     // Face 6
                     0, 0,   0, 1,   1, 1};    // Face 6

    logger->trace(logger->get() << "Before crate load");
    load();
    logger->trace(logger->get() << "Crate loaded");
}

Crate::Crate(int x, int y, int z, float size, std::string const & texture):
    Crate::Crate(x, y, z, size, "Shaders/texture.vert", "Shaders/texture.frag", texture)
{
}

Crate::~Crate()
{
    logger->trace(logger->get() << "Crate destructor");
}

void Crate::draw(glm::mat4 &projection, glm::mat4 &modelview)
{
    modelview = glm::translate(modelview, position_);

    glUseProgram(shader_->programID());

    glBindVertexArray(VAOId_);

        glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "modelview"), 1, GL_FALSE, glm::value_ptr(modelview));

        glBindTexture(GL_TEXTURE_2D, texture_->id());

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);

    glUseProgram(0);

    modelview = glm::translate(modelview, - position_);

}

void Crate::load()
{
    //VBO
    if(glIsBuffer(VBOId_) == GL_TRUE)
    {
        glDeleteBuffers(1, &VBOId_);
    }

    glGenBuffers(1, &VBOId_);
    glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

        glBufferData(GL_ARRAY_BUFFER, nbVerticesBytes() + nbTextureBytes(), 0, GL_STATIC_DRAW);


        glBufferSubData(GL_ARRAY_BUFFER, 0, nbVerticesBytes(), vertices_.data());
        glBufferSubData(GL_ARRAY_BUFFER, nbVerticesBytes(), nbTextureBytes(), textureCoord_.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //VAO
    if(glIsVertexArray(VAOId_) == GL_TRUE)
    {
        glDeleteVertexArrays(1, &VAOId_);
    }

    glGenVertexArrays(1, &VAOId_);
    glBindVertexArray(VAOId_);

        glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(nbVerticesBytes()));
            glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
}

int Crate::nbTextureBytes()
{
    return textureCoord_.size() * sizeof(float);
}

void Crate::print()
{
    logger->debug(logger->get() << "Crate:  texture name = " << texture_->file()
                << ", texture id = " << texture_->id());
}

