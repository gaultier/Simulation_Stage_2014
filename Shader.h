#ifndef DEF_SHADER
#define DEF_SHADER

/** @file
 * @brief Shader management
 * @author Philippe Gaultier
 * @version 1.0
 * @date 24/07/14
 */

// Include Windows

#ifdef WIN32
#include <GL/glew.h>


// Include Mac

#elif __APPLE__
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>


// Include UNIX/Linux

#else
#define GL3_PROTOTYPES 1
#include "Include/GL3/gl3.h"

#endif

#include <iostream>
#include <string>
#include <fstream>

/**
 * @brief The Shader class
 * @details Manages the shader resources
 */
class Shader
{
    public:

    Shader();
    Shader(Shader const &copy);
    Shader(std::string const & vertexSource, std::string const & fragmentSource);
    ~Shader();

    Shader& operator=(Shader const &copy);

    /**
     * @brief Reads the shader source files, compiles them and links them
     * @return true if it was successful, else false
     */
    bool load();

    /**
     * @brief Reads the shader source file and compiles it
     * @details Works on vertex and fragment shaders
     * @param shader The OpenGL shader id
     * @param type Shader type: vertex or fragment
     * @param fileSource The shader source file
     * @return
     */
    bool compile(GLuint &shader, GLenum type, std::string const &fileSource);

    GLuint programID() const;
    void setProgramID(const GLuint &programID);

    std::string vertexSource() const;
    void setVertexSource(const std::string& vertexSource);

    std::string fragmentSource() const;
    void setFragmentSource(const std::string& fragmentSource);

private:
    /**
     * @brief The OpenGL vertex shader id
     */
    GLuint vertexID_;

    /**
     * @brief The OpenGL fragment shader id
     */
    GLuint fragmentID_;

    /**
     * @brief The OpenGL program id resulting from the fusion of the vertex and fragment shaders
     */
    GLuint programID_;

    /**
     * @brief The vertex shader source file
     */
    std::string vertexSource_;

    /**
     * @brief The fragment shader source file
     */
    std::string fragmentSource_;
};

#endif
