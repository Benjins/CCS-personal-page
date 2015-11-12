#ifndef ARMATURE_H
#define ARMATURE_H

#pragma once

#include "BoneTransform.h"
#include "Mat4.h"
#include "ArmatureAnimation.h"

#include <vector>
#include <string>

using std::vector; using std::string;

#define MAX_ANIM_COUNT 8

struct Model;

struct Armature{
	BoneTransform bones[MAX_BONE_COUNT];
	Mat4x4 bindPoses[MAX_BONE_COUNT];
	ArmatureAnimation anim[MAX_ANIM_COUNT];

	int currentAnimIndex;
	int targetAnimIndex;
	float currentBlendTime;
	float blendTime;

	Model* model;
	int boneCount;
	int animCount;

	float time;

	Armature()
	: boneCount(0)
	, time(0.0f)
	, currentAnimIndex(0)
	, targetAnimIndex(0)
	, blendTime(0.0f)
	, currentBlendTime(0.0f)
	, model(nullptr){
	}

	BoneTransform* AddBone(BoneTransform* parent);
	void GetBoneMatrices(vector<Mat4x4>& outMatrices);
	void DebugRender();

	void Update(float deltaTime);

	BoneTransform* GetBoneByName(const string& name);
	int GetAnimationIndexByName(const string& name);
	void BlendTo(const string& animName, float time);
	void BlendToIndex(int animIndex, float time);

	void ParseStateString(const string& states);
	string EncodeStateString();
};

#endif