#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#pragma once

#include "SCTransform.h"

struct BoneTransform : SC_Transform{
	string name;

	BoneTransform()
	: SC_Transform(){
		gameObject = nullptr;
	}

	BoneTransform* GetBoneParent(){
		return static_cast<BoneTransform*>(parent);
	}
};


#endif