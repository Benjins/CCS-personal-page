#ifndef ARMATURE_ANIMATION_H
#define ARMATURE_ANIMATION_H

#pragma once

#include "Animation.h"
#include "../ext/Vector3.h"
#include "../ext/Quaternion.h"

#define MAX_BONE_COUNT 32

struct BoneAnim{
	Animation<Vector3> positionAnim;
	Animation<Quaternion> rotationAnim;
};

struct ArmatureAnimation{
	string stateName;
	BoneAnim boneAnims[MAX_BONE_COUNT];
};


#endif
