#ifndef DEF_TEXTURE
#define DEF_TEXTURE

/** @file
 * @brief Texture management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

// Include

#ifdef WIN32
#include <GL/glew.h>

#else
#define GL3_PROTOTYPES 1
#include "Include/GL3/gl3.h"

#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

/**
 * @brief The Texture class
 * @details Manages the textures resources
 */
class Texture
{
public:

    /**
     * @brief Default constructor
     */
    Texture();

    /**
     * @brief Constructor
     * @param file The image file we read from
     */
    Texture(std::string const & file);

    /**
     * @brief Copy constructor
     * @param texture The texture to copy
     */
    Texture(Texture const & texture);

    /**
     * @brief operator =
     * @param texture The texture to affect to
     * @return The texture with the updated values
     */
    Texture& operator=(Texture const &texture);

    /**
      * @brief Destructor
      */
    ~Texture();

    /**
     * @brief Reads the image file
     * @return true if the reading is succesful, else false
     */
    bool load();

    GLuint id() const;
    void setFile(const std::string &file);
    const std::string & file() const;

    /**
     * @brief Inverts the pixels of the texture image to comply to the OpenGL format
     * @param source The image in memory in SDL format
     * @return The image in memory in OpenGL format
     */
    SDL_Surface * invertPixels(SDL_Surface * source) const;

private:
    /**
     * @brief The texture image file
     */
    std::string file_;

    /**
     * @brief The OpenGL texture id
     */
    GLuint id_;

};

/**
 * @brief The TextureFactory class
 * @details Implements a shared texture pool use by all graphical textured objects
 * Part of the Flyweight pattern in use by the textured graphical object
 */
class TextureFactory
{
public:
    /**
     * @brief Gives a pointer to the texture queried by a textured graphical object
     * @details Under the hood, it checks wether the texture already exists, i.e if it has already been queried
     * for use by a textured graphical object. If it is the case, it simply gives a pointer to the existing texture.
     * Else it creates a new texture with the corresponding resource file and adds it to the pool.
     * @param file The resource texture file, which uniquely identifies it
     * @return A pointer to the queried texture
     */
    static std::shared_ptr<Texture> & createTexture(std::string const & file);

    static std::string toString();

    /**
     * @brief Clears the texture pool
     */
    static void destroyTextures();

private:
    /**
     * @brief The pool of textures shared by all textured graphical objects
     */
    static std::vector<std::shared_ptr<Texture>> textures_;
};

#endif
