#include "Texture.h"
#include "Utils.h"
#include "LogCpp/Log.h"

std::vector<std::shared_ptr<Texture>> TextureFactory::textures_;

Texture::Texture(std::string const & file):
    file_ {file},
    id_ {0}
{
    logger->trace(logger->get() << "Texture constructor: " << file_);
    load();
}

Texture::~Texture()
{
    logger->trace(logger->get() << "Texture destructor: " << file_);
    glDeleteTextures(1, &id_);
}

bool Texture::load()
{
    SDL_Surface *imageSDL = IMG_Load(file_.c_str());

    if(imageSDL == 0)
    {
        logger->error(logger->get() << "Error loading texture " << file_ << ": " << SDL_GetError());

        return false;
    }
    SDL_Surface * invertedImage = invertPixels(imageSDL);
    SDL_FreeSurface(imageSDL);

    //Delete former texture
    if(glIsTexture(id_) == GL_TRUE)
        glDeleteTextures(1, &id_);

    //Generate id
    glGenTextures(1, &id_);

    //Lock
    glBindTexture(GL_TEXTURE_2D, id_);

    //Image format
    GLenum internalFormat(0);
    GLenum format(0);

    if(invertedImage->format->BytesPerPixel == 3)
    {
        internalFormat = GL_RGB;

        if(invertedImage->format->Rmask == 0xff)
        {
            format = GL_RGB;
        }
        else
        {
            format = GL_BGR;
        }
    }
    else if(invertedImage->format->BytesPerPixel == 4)
    {
        internalFormat = GL_RGBA;

        if(invertedImage->format->Rmask == 0xff)
        {
            format = GL_RGBA;
        }
        else
        {
            format = GL_BGRA;
        }
    }
    else
    {
        logger->error(logger->get() << "Error: image internal format unknown");

        SDL_FreeSurface(invertedImage);
        return false;
    }

    //PixelCopy
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, invertedImage->w, invertedImage->h, 0, format, GL_UNSIGNED_BYTE, invertedImage->pixels);

    //Filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Unlock
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(invertedImage);

    return true;
}

GLuint Texture::id() const
{
    return id_;
}

void Texture::setFile(const std::string &file)
{
    file_ = file;
}

SDL_Surface * Texture::invertPixels(SDL_Surface * source) const
{
    SDL_Surface * invertedImage = SDL_CreateRGBSurface(0, source->w, source->h, source->format->BitsPerPixel,
                                                       source->format->Rmask, source->format->Gmask, source->format->Bmask, source->format->Amask
                                                       );

    unsigned char* pixels = (unsigned char*) source->pixels;
    unsigned char* invertedPixels = (unsigned char*) invertedImage->pixels;
    int width = source->w * source->format->BytesPerPixel;

    for(int i=0; i < source->h; i++)
    {
        for(int j=0; j < width; j++)
        {
            int pos = width * i + j;
            int invertedPos = (width * (source->h - 1 - i)) + j;
            invertedPixels[pos] = pixels[invertedPos];
        }
    }

    return invertedImage;
}

Texture::Texture(Texture const & texture)
{
    file_ = texture.file_;
    load();
}

Texture& Texture::operator=(Texture const &texture)
{
    file_ = texture.file_;
    load();

    return *this;
}

const std::string & Texture::file() const
{
    return file_;
}

std::shared_ptr<Texture> & TextureFactory::createTexture(std::string const & file)
{
    logger->debug(logger->get() << "Looking for texture " << file);

    for(auto & texture : TextureFactory::textures_)
    {
        if(texture->file() == file)
        {
            logger->debug(logger->get() << "Found texture: " << file);
            return texture;
        }
    }

    //Texture not found
    TextureFactory::textures_.push_back(std::shared_ptr<Texture>(new Texture(file)));
    logger->debug(logger->get() << "Created texture: " << file);


    return TextureFactory::textures_.back();
}

std::string TextureFactory::toString()
{
    std::string res = "Texture factory = ";

    for(const auto & t : textures_)
    {
        res += t->file() + ": " + std::to_string(t.use_count()) + "\n";
    }

    return res;
}

void TextureFactory::destroyTextures()
{
    textures_.clear();
}


