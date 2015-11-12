#ifndef VECTOR4_H
#define VECTOR4_H

#pragma once

struct Vector3;

struct Vector4{
	float w;
	float x;
	float y;
	float z;

	Vector4();
	Vector4(float _w, float _x, float _y, float _z);
	Vector4(const Vector3& vec, float _w = 1);
	Vector4(float* arrayInit);

	Vector4 operator+(const Vector4& param){
		return Vector4(w+param.w, x+param.x, y+param.y, z+param.z);
	}

	Vector4 operator*(float scale){
		return Vector4(w*scale, x*scale, y*scale, z*scale);
	}
};

#endif