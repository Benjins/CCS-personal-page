#ifndef LIGHT_H
#define LIGHT_H

#pragma once

struct Light{
	Vector3 position;
	Vector3 direction;
	float intensity;
	int id;
	bool isDirectional;
};


#endif