#include "Plane.h"
#include "Include/glm/gtx/transform.hpp"
#include "Include/glm/gtc/type_ptr.hpp"

Plane::Plane(float x, float y, float z, float width, float height, float repeatWidth, float repeatHeight, std::string const & vertexShader, std::string const & fragmentShader, std::string const &  textureFile):
    GraphicObject(x, y, z, width, vertexShader, fragmentShader),
    texture_ (nullptr)
{
    shader_->load();
    texture_ = TextureFactory::createTexture(textureFile);

    width /= 2;
    height /= 2;

    vertices_ = { -width, 0, -height,     width, 0, -height,      width, 0, height,
                  -width, 0, -height,      -width, 0, height,     width, 0, height};

    textureCoord_ = { 0, 0,      repeatWidth, 0,     repeatWidth, repeatHeight,
                      0, 0,      0, repeatHeight,    repeatWidth, repeatHeight};

    load();
}

Plane::~Plane()
{

}

void Plane::draw(glm::mat4 &projection, glm::mat4 &modelview)
{
    modelview = glm::translate(modelview, position_);

    glUseProgram(shader_->programID());

    glBindVertexArray(VAOId_);

        // Envoi des matrices
        glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader_->programID(), "modelview"), 1, GL_FALSE, glm::value_ptr(modelview));

        // Verrouillage de la texture
        glBindTexture(GL_TEXTURE_2D, texture_->id());

        // Rendu
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Déverrouillage de la texture
        glBindTexture(GL_TEXTURE_2D, 0);

     glBindVertexArray(0);

    // Désactivation du shader
    glUseProgram(0);

    modelview = glm::translate(modelview, - position_);
}

void Plane::load()
{
    //VBO
    if(glIsBuffer(VBOId_) == GL_TRUE)
    {
        glDeleteBuffers(1, &VBOId_);
    }

    glGenBuffers(1, &VBOId_);
    glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

        glBufferData(GL_ARRAY_BUFFER, nbVerticesBytes() + nbTextureBytes(), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nbVerticesBytes(), &vertices_[0]);
        glBufferSubData(GL_ARRAY_BUFFER, nbVerticesBytes(), nbTextureBytes(), &textureCoord_[0]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //VAO
    if(glIsVertexArray(VAOId_) == GL_TRUE)
      glDeleteVertexArrays(1, &VAOId_);

    glGenVertexArrays(1, &VAOId_);
    glBindVertexArray(VAOId_);

        glBindBuffer(GL_ARRAY_BUFFER, VBOId_);

            // Envoi des vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            // Envoi des coordonnées de texture
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(nbVerticesBytes()));
            glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

int Plane::nbTextureBytes()
{
    return textureCoord_.size() * sizeof(float);
}

