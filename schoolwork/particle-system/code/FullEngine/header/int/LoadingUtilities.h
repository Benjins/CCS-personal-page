#ifndef LOADINGUTILITIES_H
#define LOADINGUTILITIES_H

#pragma once

#include <string>
#include "../ext/Vector3.h"
#include "../ext/Vector2.h"
#include "Animation.h"

using std::string;

string EncodeVector3(Vector3 vec);
string EncodeVector2(Vector2 vec);
string EncodeQuaternion(Quaternion quat);
string EncodeAnimationType(AnimationType type);
string EncodeAnimationTarget(AnimationTarget target);

Vector3 ParseVector3(string encoded);
Vector2 ParseVector2(string encoded);
Quaternion ParseQuaternion(string encoded);
AnimationType ParseAnimationType(string encoded);
AnimationTarget ParseAnimationTarget(string encoded);

#endif
