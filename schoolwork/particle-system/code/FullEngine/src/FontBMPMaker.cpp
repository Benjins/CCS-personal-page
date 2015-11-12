#include "../header/int/FontBMPMaker.h"

#define FREETYPE2_STATIC
#include "../header/ext/ft2build.h"
#include FT_FREETYPE_H

#include <iostream>
#include <fstream>

using std::cout; using std::endl;
using std::ifstream; using std::ofstream;


void Blit(RGBA* dest, int destWidth, int destHeight, int _x, int _y, int width, int height, uint8* src){
	for(int y = _y; y < _y + height; y++){
		for(int x = _x; x < _x + width; x++){
			uint8 buffPix = src[(y-_y)*width + (x - _x)];
			RGBA pix = {buffPix, buffPix, buffPix, 255};
			dest[y * destWidth + x] = pix;
		}
	}
}

void ConvertTrueTypeToFUV(const string& fileName, int fontSize, FUV& readInto){
	int bitmapSize = 1024;
	readInto.imageSize = bitmapSize;
	readInto.fontSize = fontSize;

	readInto.pixels = new RGBA[bitmapSize * bitmapSize];
	RGBA* fontBitmap = readInto.pixels;

	RGBA pix = {0,0,0,255};
	for(int i = 0; i < bitmapSize * bitmapSize; i++){
		fontBitmap[i] = pix;
	}

	FT_Library lib;
	FT_Error error = FT_Init_FreeType( &lib );
	if ( error ){
	  cout << "Error: Could not initialise freetype library.\n";
	  return;
	}

	FT_Face face;  
	error = FT_New_Face( lib, fileName.c_str(), 0, &face);
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	int x = 1;
	int y = 1;

	int maxHeight = 0;

	for(unsigned char character = 32; character < 127; character++){
		if(FT_Load_Char(face, character, FT_LOAD_RENDER)){
			continue;
		}

		FT_Bitmap bitmap = face->glyph->bitmap;
		FT_Vector adv = face->glyph->advance;
		if(x + bitmap.width >= bitmapSize){
			//cout << "Jumping by maxHeight: " << maxHeight << endl;
			y += (maxHeight + 1);
			x = 1;
		}

		if(bitmap.rows > maxHeight){
			maxHeight = bitmap.rows;
		}

		Blit(fontBitmap, bitmapSize, bitmapSize, x, y, bitmap.width, bitmap.rows, bitmap.buffer);
		float x1 = x;
		float y1 = y;
		float x2 = x1 + bitmap.width;
		float y2 = y1 + bitmap.rows;
		LetterUV uvs = {x1 / bitmapSize, 
						y1 / bitmapSize, 
						x2 / bitmapSize, 
						y2 / bitmapSize, 
						((float)(adv.x >> 6)), 
						((float)(adv.y >> 6)),
						static_cast<float>(face->glyph->bitmap_left),
						static_cast<float>(face->glyph->bitmap_top)};

		readInto.uvs[character] = uvs;
		x += bitmap.width + 1;
	}
}

void ExportFUV(const FUV& fuv, const string& fileName){
	ofstream fileOut;
	fileOut.open(fileName, std::ios::out | ofstream::binary);
	if(!fileOut.good()){
		cout << "Trouble opening file " << fileName << " for output.\n";
		return;
	}

	char buffer[4] = "FUV";
	fileOut.write(buffer, 4);

	int versionNumber = CURRENT_FUV_VERSION;
	char* versionNumberCast = (char*)(void*)&versionNumber;
	fileOut.write(versionNumberCast, 4);

	char* imgSizeCast = (char*)(void*)&(fuv.imageSize);
	fileOut.write(imgSizeCast, 4);

	char* fontSizeCast = (char*)(void*)&(fuv.fontSize);
	fileOut.write(fontSizeCast, 4);

	cout << "About to write pixels.\n";
	for(int i =  0; i < fuv.imageSize * fuv.imageSize; i++){
		RGBA pix = fuv.pixels[i];
		char* data = reinterpret_cast<char*>(&pix);
		fileOut.write(data, 4);
	}

	char* uvBuffer;
	for(int i = 0; i < 128; i++){
		LetterUV uv = fuv.uvs[i];
		uvBuffer = reinterpret_cast<char*>(&uv);
		fileOut.write(uvBuffer, sizeof(LetterUV));
		//fileOut << uv.xMin << uv.yMin << uv.xMax << uv.yMax << uv.xAdvance << uv.yAdvance;
	}


	cout << "Done exporting.\n";
	fileOut.close();
}

void ImportFUV(const string& fileName, FUV& readInto){
	ifstream fileIn;
	fileIn.open(fileName, std::ios::in | ifstream::binary);
	if(!fileIn.good()){
		cout << "Trouble opening file " << fileName << " for output.\n";
		return;
	}

	readInto.fileName = fileName;

	char buffer[4];
	fileIn.read(buffer, 4); //"FUV\0"
	if(strcmp(buffer, "FUV") != 0){
		cout << "Warning: The file " << fileName << " may not be an FUV file.";
	}

	fileIn.read(buffer, 4); //imageSize
	int* numberBufer = reinterpret_cast<int*>(buffer);
	int versionNumber = *numberBufer;
	//cout << "Version number: " << versionNumber << endl;

	fileIn.read(buffer, 4);
	int imgSize = *numberBufer;

	//TODO: Check for non-POT texture size
	if(imgSize < 0 || imgSize > 4096){
		cout << "Error: tried to import FUV from " << fileName << " which had an image size of: " << imgSize << endl;
		return;
	}

	readInto.imageSize = imgSize;
	readInto.pixels = new RGBA[imgSize * imgSize];

	fileIn.read(buffer, 4);
	readInto.fontSize = *numberBufer;
	
	for(int i = 0; i < imgSize * imgSize; i++){
		fileIn.read(buffer, 4);
		RGBA pix = *reinterpret_cast<RGBA*>(buffer);
		readInto.pixels[i] = pix;
	}

	for(int i = imgSize*imgSize - 25; i < imgSize * imgSize; i++){
		RGBA pix = readInto.pixels[i];
	}

	char uvBuffer[sizeof(LetterUV)];
	for(int i = 0; i < 128; i++){
		LetterUV uv;
		fileIn.read(uvBuffer, sizeof(LetterUV));
		uv = *reinterpret_cast<LetterUV*>(uvBuffer);
		readInto.uvs[i] = uv;
		int x = 5;
	}

	fileIn.close();
}

void Swap2Bytes(void* data){
	uint8* dataCast = (uint8*)data;

	uint8 temp = dataCast[0];
	dataCast[0] = dataCast[1];
	dataCast[1] = temp;
}

void Swap4Bytes(void* data){
	uint8* x = (uint8*)data;
	*x = (*x & 0x0000FFFF) << 16 | (*x & 0xFFFF0000) >> 16;
	*x = (*x & 0x00FF00FF) << 8  | (*x & 0xFF00FF00) >> 8;  
}