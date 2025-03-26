#include "Texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>


Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
	
}

Texture::~Texture()
{
	
}


bool Texture::Load(const std::string& fileName)
{
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF) == 0)
    {
        SDL_Log("Failed to initialize SDL2_image: %s", IMG_GetError());
        return false;
    }
    
    SDL_Surface* image = IMG_Load(fileName.c_str());
    if (!image)
    {
        SDL_Log("Failed to load image %s: %s", fileName.c_str(), IMG_GetError());
        return false;
    }

    SDL_Surface* tmpImage = SDL_CreateRGBSurface(
        0, image->w, image->h, 32,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
    );
    if (!tmpImage)
    {
        SDL_Log("Failed to create temporary surface: %s", SDL_GetError());
        SDL_FreeSurface(image);
        return false;
    }

    SDL_Surface* convertedImage = SDL_ConvertSurface(image, tmpImage->format, 0);
    SDL_FreeSurface(image);
    SDL_FreeSurface(tmpImage);

    if (!convertedImage)
    {
        SDL_Log("Failed to convert surface: %s", SDL_GetError());
        return false;
    }

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, convertedImage->w, convertedImage->h,
        0, GL_RGBA, GL_UNSIGNED_BYTE, convertedImage->pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mWidth = convertedImage->w;
    mHeight = convertedImage->h;

    SDL_FreeSurface(convertedImage);
    return true;
}
// レンダリング用テクスチャを生成
void Texture::CreateForRendering(int w, int h, unsigned int format)
{
    mWidth = w;
    mHeight = h;
    // テクスチャ生成
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    // 空のテクスチャ
    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


// テクスチャ削除
void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

// アクティブ化（Bind）
void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
