#include "../header/int/AudioSystem.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

using std::cout; using std::endl;

void AudioClip::LoadFromWavFile(const char* name){
	char type[4];
	int size,chunkSize;
	short formatType,channels;
	int sampleRate, avgBytesPerSec;
	short bytesPerSample,bitsPerSample;
  int dataSize=0;

	FILE* wavFile;
  	wavFile = fopen (name,"rb");

  	if(wavFile == nullptr){
  		cout << "Could not open file: '" << name << "'\n";
  		return;
  	}

  	fread(type, sizeof(char), 4, wavFile);
  	if(type[0] != 'R' || type[1] != 'I' ||  type[2] != 'F' ||  type[3] != 'F'){
  		cout << "\nError reading wav file, format not recognised.\n";
  		return;
  	}

  	fread(&size, sizeof(int), 1, wavFile);

  	fread(type, sizeof(char), 4, wavFile);
  	if(type[0] != 'W' || type[1] != 'A' ||  type[2] != 'V' ||  type[3] != 'E'){
  		cout << "\nError reading wav file, format not recognised.\n";
  		return;
  	}

  	fread(type, sizeof(char), 4, wavFile);
  	if(type[0] != 'f' || type[1] != 'm' ||  type[2] != 't' ||  type[3] != ' '){
  		cout << "\nError reading wav file, format not recognised.\n";
  		return;
  	}

  	fread(&chunkSize,      sizeof(int), 1, wavFile);
  	fread(&formatType,     sizeof(short), 1, wavFile);
  	fread(&channels,       sizeof(short), 1, wavFile);
  	fread(&sampleRate,     sizeof(int), 1, wavFile);
  	fread(&avgBytesPerSec, sizeof(int), 1, wavFile);
  	fread(&bytesPerSample, sizeof(short), 1, wavFile);
  	fread(&bitsPerSample,  sizeof(short), 1, wavFile);

  	fread(type, sizeof(char), 4, wavFile);

  	if(strncmp(type, "data", 4) != 0){
  		if(strncmp(type, "LIST", 4) == 0){
  			int length;
  			fread(&length, sizeof(int), 1, wavFile);

  			char* listData = new char[length+1];

  			fread(listData, sizeof(char), length, wavFile);
  			listData[length] = 0;

  			printf("Data is LIST, followed by %d byteS: '%s'", length, listData);

  			delete listData;

  			fread(type, sizeof(char), 4, wavFile);
  			printf("\nAfter that, there's: '%c%c%c%c'\n", type[0], type[1], type[2], type[3]);
  		}
  	}

  	if(strncmp(type, "data", 4) != 0){
  		printf("\nError: Data missing from wav file.\n");
  		return;
  	}

  	fread(&dataSize, sizeof(int), 1, wavFile);

  	unsigned char* dataBuffer = new unsigned char[dataSize];
  	fread(dataBuffer, sizeof(unsigned char), dataSize, wavFile);

  	fclose(wavFile);

  	frequency = sampleRate;

  	alGenBuffers(1, &buffer);
  	alGenSources(1, &source);

  	if(bitsPerSample == 8){
  		if(channels == 1){ 
			format = AL_FORMAT_MONO8;
  		}
  		else if(channels == 2){
  			format = AL_FORMAT_STEREO8;
  		}
  		else{
  			cout << "\nError: nvalid number of channels: " << channels << "\n";
  			return;
  		}
  	}
  	else if(bitsPerSample == 16){
  		if(channels == 1){
			format = AL_FORMAT_MONO16;
  		}
  		else if(channels == 2){
  			format = AL_FORMAT_STEREO16;
  		}
  		else{
  			cout << "\nError: nvalid number of channels: " << channels << "\n";
  			return;
  		}
  	}
  	else{
  		cout << "Error: Invalid bits per sample: " << bitsPerSample << "\n";
  	}

    clipFileName = name;
  	alBufferData(buffer, format, dataBuffer, dataSize, frequency);

  	delete[] dataBuffer;

  	ALfloat sourcePos[] = {0,0,0};
  	ALfloat sourceVel[] = {0,0,0};

  	alSourcei(source, AL_BUFFER, buffer);
  	alSourcef(source, AL_PITCH, 1);
  	alSourcef(source, AL_GAIN,  1);

  	alSourcefv(source, AL_POSITION, sourcePos);
  	alSourcefv(source, AL_VELOCITY, sourceVel);
  	alSourcei(source,  AL_LOOPING, AL_TRUE);
}

void AudioSystem::Initialise(){
	device = alcOpenDevice(nullptr);

	if(!device){
		cout << "\nError: no sound device, could not initialise OpenAL.\n";
		return;
	}

	context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);
	if(!context){
		cout << "\nError: no sound context found, could not initialise OpenAL.\n";
		return;
	}
  
  printf("OpenAL Version: %s\n", alGetString(AL_VERSION));

	ALfloat listenerPos[] = {0,0,0};
	ALfloat listenerVel[] = {0,0,0};
	ALfloat listenerOri[] = {0,0,-1,0,1,0};

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
}
