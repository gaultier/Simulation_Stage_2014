#include "Shader.h"
#include "spdlog/include/spdlog/spdlog.h"
#include <exception>
#include <string>

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
      if (glIsShader(vertexID_)) glDeleteShader(vertexID_);

      if (glIsShader(fragmentID_)) glDeleteShader(fragmentID_);

      if (glIsProgram(programID_)) glDeleteProgram(programID_);

      if (!compile(vertexID_, GL_VERTEX_SHADER, vertexSource_))
        throw std::runtime_error("Shader vertex compilation error: " + vertexSource_);

      if (!compile(fragmentID_, GL_FRAGMENT_SHADER, fragmentSource_))
        throw std::runtime_error("Shader fragment compilation error: " + fragmentSource_);

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
      GLint linkSucess = 0;
      glGetProgramiv(programID_, GL_LINK_STATUS, &linkSucess);

      if (!linkSucess)
      {
        GLint errorSize(0);
        glGetProgramiv(programID_, GL_INFO_LOG_LENGTH, &errorSize);

        char error[errorSize + 1];

        glGetShaderInfoLog(programID_, errorSize, &errorSize, error);

        error[errorSize] = '\0';

        throw std::runtime_error("Shader link error: " + std::string(error));
      }

      return true;
    }


    bool Shader::compile(GLuint &shader, GLenum type, std::string const &sourceFile)
    {
      shader = glCreateShader(type);

      if (!shader) throw std::runtime_error("Shader type does not exist: " + std::to_string(type));

      std::ifstream file(sourceFile.c_str());
      if (!file) throw std::runtime_error("Shader: cannot find the source file " + sourceFile);

      std::string line;
      std::string sourceCode;

      while(getline(file, line))
      sourceCode += line + '\n';

      file.close();

      const GLchar* chaineCodeSource = sourceCode.c_str();

      glShaderSource(shader, 1, &chaineCodeSource, 0);

      glCompileShader(shader);

      GLint compilationSuccessful = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccessful);

      if (!compilationSuccessful)
      {
        GLint errorSize(0);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorSize);

        char error[errorSize + 1];

        glGetShaderInfoLog(shader, errorSize, &errorSize, error);
        error[errorSize] = '\0';

        throw std::runtime_error("Shader link error '" + sourceFile + "': " + std::string(error));
      }
      return true;
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
