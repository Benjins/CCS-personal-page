#ifndef FONT_BMP_MAKER_H
#define FONT_BMP_MAKER_H

#pragma once

#define CURRENT_FUV_VERSION 1

#include <string>

using std::string;

typedef unsigned char uint8;

/*
	FUV file format:
	4 bytes: "FUV\0"
	4 bytes: Version number
	4 bytes: image size 
	4 bytes: font size
	NOTE: offset to uvs = [image size] * [image size] * 4 + 8(for header)

	image pixels, row by row, 4 bytes/pixel.
	
	128 LetterUVs data, 32 bytes each
*/

struct RGBA{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

//The UV coordinates for each letter, as well as the amount to advance by
struct LetterUV{
	float xMin;
	float yMin;

	float xMax;
	float yMax;

	float xAdvance;
	float yAdvance;

	float left;
	float top;
};

//The structure that holds the bitmap of the font, as well as the UV information
//needed to render each character on it
struct FUV{
	LetterUV uvs[128];
	string fileName;
	RGBA* pixels;
	int imageSize;
	int fontSize;

	FUV(){
		pixels = nullptr;
	}

	~FUV(){
		if(pixels != nullptr){
			delete[] pixels;
		}
	}
};

//Place an image on another image
void Blit(RGBA* dest, int destWidth, int destHeight, int _x, int _y, int width, int height, uint8* src);

//Import a font file, render it at a size, and store the information into an FUV
void ConvertTrueTypeToFUV(const string& fileName, int fontSize, FUV& readInto);

//Save the FUV as a file
void ExportFUV(const FUV& fuv, const string& fileName);

//Read in an FUV from a file
void ImportFUV(const string& fileName, FUV& readInto);

//Utility, not currently used
void Swap2Bytes(void* data);
void Swap4Bytes(void* data);

#endif