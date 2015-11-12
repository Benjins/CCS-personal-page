#include "../header/int/Vector4.h"
#include "../header/ext/Vector3.h"



Vector4::Vector4(){
	w = x = y = z = 0;
}

Vector4::Vector4(float _w, float _x, float _y, float _z){
	w = _w;
	x = _x;
	y = _y; 
	z = _z;
}

Vector4::Vector4(const Vector3& vec, float _w){
	w = vec.x;
	x = vec.y;
	y = vec.z;
	z = _w;
}

Vector4::Vector4(float* arrayInit){
	w = arrayInit[0];
	x = arrayInit[1];
	y = arrayInit[2];
	z = arrayInit[3];
}
