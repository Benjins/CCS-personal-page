#ifndef META_TYPES_H
#define META_TYPES_H
#include "../header/int/Macros.h"
#include "../header/int/MetaTypeInfo.h"
#if defined(_MSC_VER)
#define OFFSET_OF(type,field) static_cast<int>((size_t)&(((type*)0)->field))
#else
#include <stddef.h>
#define OFFSET_OF offsetof
#endif
MetaMemberInfo memberInfo_DoorComponent[] = {
	{"isLocked", OFFSET_OF(DoorComponent, isLocked), MetaType_bool},
	{"isOpen", OFFSET_OF(DoorComponent, isOpen), MetaType_bool},
};
MetaMemberInfo memberInfo_AnimationControlTest[] = {
	{"timeOut", OFFSET_OF(AnimationControlTest, timeOut), MetaType_float},
	{"isIdle", OFFSET_OF(AnimationControlTest, isIdle), MetaType_bool},
};
MetaMemberInfo memberInfo_AudioComponent[] = {
	{"clipName", OFFSET_OF(AudioComponent, clipName), MetaType_string},
	{"volume", OFFSET_OF(AudioComponent, volume), MetaType_float},
	{"loop", OFFSET_OF(AudioComponent, loop), MetaType_bool},
	{"autoPlay", OFFSET_OF(AudioComponent, autoPlay), MetaType_bool},
};
MetaMemberInfo memberInfo_CameraControl[] = {
	{"speed", OFFSET_OF(CameraControl, speed), MetaType_float},
	{"velocity", OFFSET_OF(CameraControl, velocity), MetaType_float},
	{"isGrounded", OFFSET_OF(CameraControl, isGrounded), MetaType_bool},
	{"health", OFFSET_OF(CameraControl, health), MetaType_float},
	{"stepDelay", OFFSET_OF(CameraControl, stepDelay), MetaType_float},
	{"timeMoving", OFFSET_OF(CameraControl, timeMoving), MetaType_float},
	{"characterHeight", OFFSET_OF(CameraControl, characterHeight), MetaType_float},
	{"groundedAdjustment", OFFSET_OF(CameraControl, groundedAdjustment), MetaType_float},
	{"prevX", OFFSET_OF(CameraControl, prevX), MetaType_int},
	{"prevY", OFFSET_OF(CameraControl, prevY), MetaType_int},
	{"xRot", OFFSET_OF(CameraControl, xRot), MetaType_float},
	{"yRot", OFFSET_OF(CameraControl, yRot), MetaType_float},
};
MetaMemberInfo memberInfo_BulletComponent[] = {
	{"speed", OFFSET_OF(BulletComponent, speed), MetaType_float},
	{"time", OFFSET_OF(BulletComponent, time), MetaType_float},
	{"timeLimit", OFFSET_OF(BulletComponent, timeLimit), MetaType_float},
};
MetaMemberInfo memberInfo_PathNodeComponent[] = {
	{"nodeId", OFFSET_OF(PathNodeComponent, nodeId), MetaType_int},
};
MetaMemberInfo memberInfo_AnimationComponent[] = {
	{"currentTime", OFFSET_OF(AnimationComponent, currentTime), MetaType_float},
	{"loop", OFFSET_OF(AnimationComponent, loop), MetaType_bool},
	{"playAutomatically", OFFSET_OF(AnimationComponent, playAutomatically), MetaType_bool},
	{"isPlaying", OFFSET_OF(AnimationComponent, isPlaying), MetaType_bool},
};
MetaMemberInfo memberInfo_LightComponent[] = {
	{"intensity", OFFSET_OF(LightComponent, intensity), MetaType_float},
	{"id", OFFSET_OF(LightComponent, id), MetaType_int},
	{"isDirectional", OFFSET_OF(LightComponent, isDirectional), MetaType_bool},
};
MetaMemberInfo memberInfo_SimpleAnimation[] = {
	{"currTime", OFFSET_OF(SimpleAnimation, currTime), MetaType_float},
};
MetaMemberInfo memberInfo_AnimateUVOffset[] = {
	{"currentTime", OFFSET_OF(AnimateUVOffset, currentTime), MetaType_float},
};
MetaMemberInfo memberInfo_FireGun[] = {
	{"bulletForce", OFFSET_OF(FireGun, bulletForce), MetaType_float},
};
MetaMemberInfo memberInfo_RotateConstantly[] = {
	{"rotationSpeed", OFFSET_OF(RotateConstantly, rotationSpeed), MetaType_float},
};
MetaMemberInfo memberInfo_EnemyComp[] = {
	{"speed", OFFSET_OF(EnemyComp, speed), MetaType_float},
	{"health", OFFSET_OF(EnemyComp, health), MetaType_int},
	{"maxHealth", OFFSET_OF(EnemyComp, maxHealth), MetaType_int},
	{"reloadTime", OFFSET_OF(EnemyComp, reloadTime), MetaType_float},
};
MetaMemberInfo memberInfo_OscillateUp[] = {
	{"time", OFFSET_OF(OscillateUp, time), MetaType_float},
	{"frameCount", OFFSET_OF(OscillateUp, frameCount), MetaType_int},
};
MetaMemberInfo memberInfo_BoxCollider[] = {
	{"position", OFFSET_OF(BoxCollider, position), MetaType_Vector3},
	{"size", OFFSET_OF(BoxCollider, size), MetaType_Vector3},
};
MetaMemberInfo memberInfo_SphereCollider[] = {
	{"position", OFFSET_OF(SphereCollider, position), MetaType_Vector3},
	{"radius", OFFSET_OF(SphereCollider, radius), MetaType_float},
};
MetaMemberInfo memberInfo_RigidBody[] = {
	{"mass", OFFSET_OF(RigidBody, mass), MetaType_float},
	{"useGravity", OFFSET_OF(RigidBody, useGravity), MetaType_bool},
	{"isKinematic", OFFSET_OF(RigidBody, isKinematic), MetaType_bool},
	{"time", OFFSET_OF(RigidBody, time), MetaType_float},
};
enum MetaStructTypes {
metaStruct_DoorComponent,
metaStruct_AnimationControlTest,
metaStruct_AudioComponent,
metaStruct_CameraControl,
metaStruct_BulletComponent,
metaStruct_PathNodeComponent,
metaStruct_AnimationComponent,
metaStruct_LightComponent,
metaStruct_SimpleAnimation,
metaStruct_AnimateUVOffset,
metaStruct_FireGun,
metaStruct_RotateConstantly,
metaStruct_EnemyComp,
metaStruct_OscillateUp,
metaStruct_BoxCollider,
metaStruct_SphereCollider,
metaStruct_RigidBody,
metaStruct_PlayerComponent,
metaStruct_TestComp,
metaStruct_HitComponent,
metaStruct_MatChangeOnHit,
metaStruct_Collider,
};
MetaStructInfo genMetaStructInfo[] = {
{"DoorComponent", memberInfo_DoorComponent, ARRAY_COUNT(memberInfo_DoorComponent)},
{"AnimationControlTest", memberInfo_AnimationControlTest, ARRAY_COUNT(memberInfo_AnimationControlTest)},
{"AudioComponent", memberInfo_AudioComponent, ARRAY_COUNT(memberInfo_AudioComponent)},
{"CameraControl", memberInfo_CameraControl, ARRAY_COUNT(memberInfo_CameraControl)},
{"BulletComponent", memberInfo_BulletComponent, ARRAY_COUNT(memberInfo_BulletComponent)},
{"PathNodeComponent", memberInfo_PathNodeComponent, ARRAY_COUNT(memberInfo_PathNodeComponent)},
{"AnimationComponent", memberInfo_AnimationComponent, ARRAY_COUNT(memberInfo_AnimationComponent)},
{"LightComponent", memberInfo_LightComponent, ARRAY_COUNT(memberInfo_LightComponent)},
{"SimpleAnimation", memberInfo_SimpleAnimation, ARRAY_COUNT(memberInfo_SimpleAnimation)},
{"AnimateUVOffset", memberInfo_AnimateUVOffset, ARRAY_COUNT(memberInfo_AnimateUVOffset)},
{"FireGun", memberInfo_FireGun, ARRAY_COUNT(memberInfo_FireGun)},
{"RotateConstantly", memberInfo_RotateConstantly, ARRAY_COUNT(memberInfo_RotateConstantly)},
{"EnemyComp", memberInfo_EnemyComp, ARRAY_COUNT(memberInfo_EnemyComp)},
{"OscillateUp", memberInfo_OscillateUp, ARRAY_COUNT(memberInfo_OscillateUp)},
{"BoxCollider", memberInfo_BoxCollider, ARRAY_COUNT(memberInfo_BoxCollider)},
{"SphereCollider", memberInfo_SphereCollider, ARRAY_COUNT(memberInfo_SphereCollider)},
{"RigidBody", memberInfo_RigidBody, ARRAY_COUNT(memberInfo_RigidBody)},
{"PlayerComponent", nullptr, 0},
{"TestComp", nullptr, 0},
{"HitComponent", nullptr, 0},
{"MatChangeOnHit", nullptr, 0},
{"Collider", nullptr, 0},
};

#endif
