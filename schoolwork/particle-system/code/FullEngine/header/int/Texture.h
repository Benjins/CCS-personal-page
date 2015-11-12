#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

#include <string>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

using std::string;

struct RGBApixel {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
};

struct ResourceManager;

struct Texture{
	//mip maps
	//bilinear
	//love me
	
	string fileName;
	GLenum textureTarget;
    GLuint textureObj;

	RGBApixel* pixelData;
	int width;
	int height;
	
	Texture();
	Texture(int _width, int _height);
	Texture(GLenum TextureTarget, const string& FileName);

	void Load(GLenum TextureTarget = GL_TEXTURE0, GLenum textureType = GL_TEXTURE_2D, bool apply = true);

	void SetPixel(int x, int y, float r, float g, float b);
	void SetPixel(int x, int y, RGBApixel col);
	void GetPixel(int x, int y);
	void Apply(GLenum TextureTarget = GL_TEXTURE0, GLenum textureType = GL_TEXTURE_2D);

    void Bind(GLenum TextureUnit);

	void Release(ResourceManager* manager);

	~Texture();
};


#endif