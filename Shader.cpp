#include "Shader.h"
#include "LogCpp/Log.h"

Shader::Shader() :
    vertexID_ {0},
    fragmentID_ {0},
    programID_ {0},
    vertexSource_ {},
    fragmentSource_ {}
{
}

Shader::Shader(Shader const &copy)
{
    vertexSource_ = copy.vertexSource_;
    fragmentSource_ = copy.fragmentSource_;

    load();
}


Shader::Shader(std::string const & vertexSource, std::string const & fragmentSource) :
    vertexID_ {0},
    fragmentID_ {0},
    programID_ {0},
    vertexSource_ {vertexSource},
    fragmentSource_ {fragmentSource}
{
}


Shader::~Shader()
{
    glDeleteShader(vertexID_);
    glDeleteShader(fragmentID_);
    glDeleteProgram(programID_);
}

Shader& Shader::operator=(Shader const &copy)
{
    vertexSource_ = copy.vertexSource_;
    fragmentSource_ = copy.fragmentSource_;

    load();

    return *this;
}


bool Shader::load()
{
    if(glIsShader(vertexID_) == GL_TRUE)
        glDeleteShader(vertexID_);

    if(glIsShader(fragmentID_) == GL_TRUE)
        glDeleteShader(fragmentID_);

    if(glIsProgram(programID_) == GL_TRUE)
        glDeleteProgram(programID_);

    if(!compile(vertexID_, GL_VERTEX_SHADER, vertexSource_))
        return false;

    if(!compile(fragmentID_, GL_FRAGMENT_SHADER, fragmentSource_))
        return false;

    programID_ = glCreateProgram();

    //Fusion
    glAttachShader(programID_, vertexID_);
    glAttachShader(programID_, fragmentID_);


    // Lock
    glBindAttribLocation(programID_, 0, "in_Vertex");
    glBindAttribLocation(programID_, 1, "in_Color");
    glBindAttribLocation(programID_, 2, "in_TexCoord0");


    // Link
    glLinkProgram(programID_);


    // Check link
    GLint linkError(0);
    glGetProgramiv(programID_, GL_LINK_STATUS, &linkError);

    if(linkError != GL_TRUE)
    {
        GLint errorSize(0);
        glGetProgramiv(programID_, GL_INFO_LOG_LENGTH, &errorSize);

        char *error = new char[errorSize + 1];

        glGetShaderInfoLog(programID_, errorSize, &errorSize, error);
        error[errorSize] = '\0';

        logger->error(logger->get() << "Shader link error: " << error);

        delete[] error;
        glDeleteProgram(programID_);

        return false;
    }
    else
    {
        return true;
    }
}


bool Shader::compile(GLuint &shader, GLenum type, std::string const &sourceFile)
{
    shader = glCreateShader(type);

    if( ! shader)
    {
        logger->error(logger->get() << "Shader type does not exist: " << type);

        return false;
    }

    std::ifstream file(sourceFile.c_str());


    // Test d'ouverture

    if(!file)
    {
        logger->error(logger->get() << "Shader: cannot find the source file " << sourceFile);

        glDeleteShader(shader);

        return false;
    }


    std::string line;
    std::string sourceCOde;


    while(getline(file, line))
        sourceCOde += line + '\n';

    file.close();

    const GLchar* chaineCodeSource = sourceCOde.c_str();

    glShaderSource(shader, 1, &chaineCodeSource, 0);

    glCompileShader(shader);

    GLint compilationError(0);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationError);

    if(compilationError != GL_TRUE)
    {
        GLint errorSize(0);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorSize);

        char *error = new char[errorSize + 1];

        glGetShaderInfoLog(shader, errorSize, &errorSize, error);
        error[errorSize] = '\0';

        logger->error(logger->get() << "Shader compilation error: " << error);

        delete[] error;
        glDeleteShader(shader);

        return false;
    }
    else
    {
        return true;
    }
}

GLuint Shader::programID() const
{
    return programID_;
}

void Shader::setProgramID(const GLuint &programID)
{
    programID_ = programID;
}
std::string Shader::vertexSource() const
{
    return vertexSource_;
}

void Shader::setVertexSource(const std::string& vertexSource)
{
    vertexSource_ = vertexSource;
}
std::string Shader::fragmentSource() const
{
    return fragmentSource_;
}

void Shader::setFragmentSource(const std::string& fragmentSource)
{
    fragmentSource_ = fragmentSource;
}



