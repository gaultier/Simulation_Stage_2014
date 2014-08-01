#ifndef DEF_CRATE
#define DEF_CRATE

/** @file
 * @brief Crate (i.e textured Cube) management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 25/07/14
 */

#include "Cube.h"
#include "Texture.h"

#include <string>
#include <memory>

/**
 * @brief The Crate class
 * @details Textured cube. Uses the Flyweight pattern with a shared texture pool
 */
class Crate: public Cube
{
public:
    Crate(float x, float y, float z, float size, std::string const & vertexShader, std::string const & fragmentShader, std::string const & texture);
    Crate(int x, int y, int z, float size, std::string const & texture);
    virtual ~Crate();
    void draw(glm::mat4 &projection, glm::mat4 &modelview);
    void load();

    /**
     * @brief Gives the memory size of the texture coordinates in bytes
     * @return The memory size of the texture coordinates in bytes
     */
    int nbTextureBytes();

    void print();

protected:

    /**
     * @brief The texture manager
     * @details A crate only has 1 texture which is repeated on all 6 faces
     */
    std::shared_ptr<Texture> texture_;

    /**
     * @brief The texture coordinates stored in a 1D array for OpenGL conviniency
     */
    std::vector<float> textureCoord_;
};

#endif
