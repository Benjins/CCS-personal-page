#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include <stdio.h>
#include <iostream>

using std::cout;

#if defined(_WIN32) || defined(_WIN64)

#pragma pack(1)
struct BitMapHeader{
	short fileTag;
	int fileSize;
	short reservedA;
	short reservedB;
	int imageDataOffset;

	int headerSize;
	int imageWidth;
	int imageHeight;
	short numColorPlanes;
	short bitDepth;
	int compressionMethod;
	int imageDataSize;
	int horizontalResolution;
	int verticalResolution;
	int numPaletteColors;
	int numImportantColors;
};
#pragma pack()

#else
typedef struct __attribute((packed))__{
	short fileTag;
	int fileSize;
	short reservedA;
	short reservedB;
	int imageDataOffset;

	int headerSize;
	int imageWidth;
	int imageHeight;
	short numColorPlanes;
	short bitDepth;
	int compressionMethod;
	int imageDataSize;
	int horizontalResolution;
	int verticalResolution;
	int numPaletteColors;
	int numImportantColors;
} BitMapHeader;
#endif

Texture::Texture(){
	fileName = "";
	pixelData = nullptr;
	width = 0;
	height = 0;
}

Texture::Texture(int _width, int _height){
	fileName = "";
	width = _width;
	height = _height;

	textureTarget = GL_TEXTURE_2D;
	pixelData = new RGBApixel[width*height];

	glGenTextures(1, &textureObj);
    Apply();
}

Texture::Texture(GLenum TextureTarget, const std::string& _fileName){
    textureTarget = TextureTarget;
    fileName      = _fileName;
	pixelData = nullptr;
}

//Requires OpenGL context
void Texture::Load(GLenum TextureTarget, GLenum textureType, bool apply){
	if(fileName.find("Sky_") != string::npos){
		int xxs = 0;
	}

	FILE* bmpFile = fopen(fileName.c_str(), "rb");
	BitMapHeader header;	
	fread(&header, sizeof(header), 1, bmpFile);

	int imageDataSize = header.imageDataSize;
	if(imageDataSize == 0){
		imageDataSize = header.imageHeight * header.imageWidth * header.bitDepth / 8;
	}

	fseek(bmpFile, header.imageDataOffset - sizeof(header), SEEK_CUR);

	unsigned char* imgBuffer = new unsigned char[imageDataSize];
	fread(imgBuffer, 1, imageDataSize, bmpFile);
	fclose(bmpFile);

	width = header.imageWidth;
	height = header.imageHeight;

	if(pixelData != nullptr){
		delete[] pixelData;
	}

	pixelData = new RGBApixel[width*height];

	for(int i = 0; i < width*height; i++){
		pixelData[i].Blue = imgBuffer[i*3];
		pixelData[i].Green = imgBuffer[i*3+1];
		pixelData[i].Red = imgBuffer[i*3+2];
		pixelData[i].Alpha = 255;
	}

	delete[] imgBuffer;

	if(apply){
		glGenTextures(1, &textureObj);
		Apply(TextureTarget, textureType);
	}
}

void Texture::SetPixel(int x, int y, float r, float g, float b){
	RGBApixel pix;
	pix.Red   = (unsigned char)(255*r);
	pix.Green = (unsigned char)(255*g);
	pix.Blue  = (unsigned char)(255*b);
	pix.Alpha = 255;

	int index = y * width + x;
	pixelData[index] = pix;
}

void Texture::SetPixel(int x, int y, RGBApixel col){
	int index = y * width + x;
	pixelData[index] = col;
}

void Texture::GetPixel(int x, int y){
	//Not implemented yet.
}

void Texture::Apply(GLenum TextureTarget, GLenum textureType){
	if(pixelData != NULL){
		glActiveTexture(TextureTarget);
		glBindTexture(textureType, textureObj);
		glTexImage2D(textureTarget, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void Texture::Bind(GLenum TextureUnit){
    glActiveTexture(TextureUnit);
    glBindTexture(textureTarget, textureObj);
}

void Texture::Release(ResourceManager* manager){
	glDeleteTextures(1, &textureObj);
	if(pixelData != NULL){
		delete[] pixelData;
		pixelData = NULL;
	}

	fileName = "";
}

Texture::~Texture(){
	fileName="";
	if(pixelData != NULL){
		delete[] pixelData;
	}
}