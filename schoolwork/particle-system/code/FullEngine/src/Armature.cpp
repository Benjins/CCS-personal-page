#include "../header/int/Armature.h"
#include "../header/int/GameObject.h"
#include "../header/int/Model.h"
#include "../header/int/Vector4.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

BoneTransform* Armature::AddBone(BoneTransform* parent){
	if(boneCount >= MAX_BONE_COUNT){
		cout << "\n\nError: At max bone count.\n";
		return nullptr;
	}
	BoneTransform* newBone = &bones[boneCount];
	newBone->SetParent(parent);
	boneCount++;
	return newBone;
}

void Armature::GetBoneMatrices(vector<Mat4x4>& outMatrices){
	outMatrices.resize(boneCount);
	for(int i = 0; i < boneCount; i++){
		Mat4x4 localToGlobal = bones[i].LocalToGlobalMatrix();
		outMatrices[i] =  localToGlobal * bindPoses[i];
	}
}

void Armature::DebugRender(){
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	GLint colorLoc = glGetUniformLocation(prog, "_color");

	glLineWidth(4);

	SC_Transform trans;
	trans.rotation = Quaternion(X_AXIS, -3.14159265f/2);
	trans.position = Vector3(0,0,-0.3091092f);

	vector<Mat4x4> boneMats;
	GetBoneMatrices(boneMats);
	boneMats.resize(boneMats.size() + 1);

	Mat4x4 modelMatrix = model->gameObject->transform.LocalToGlobalMatrix();

	for(int i = 0; i < boneCount; i++){
		Mat4x4 matrix = modelMatrix * boneMats[i];

		Vector4 pos4 = matrix * Vector4(0,0,0,1);
		Vector3 pos = Vector3(pos4.w, pos4.x, pos4.y);

		Vector4 for4 = matrix * Vector4(0,0,1,0);
		Vector3 forward = pos + Vector3(for4.w, for4.x, for4.y);

		Vector4 up4 = matrix * Vector4(0,1,0,0);
		Vector3 up = pos + Vector3(up4.w, up4.x, up4.y);

		Vector4 right4 = matrix * Vector4(1,0,0,0);
		Vector3 right = pos + Vector3(right4.w, right4.x, right4.y);

		glUniform4f(colorLoc, 0, 0, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(forward.x, forward.y, forward.z);
		glEnd();

		glUniform4f(colorLoc, 0, 1, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(up.x, up.y, up.z);
		glEnd();

		glUniform4f(colorLoc, 1, 0, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(right.x, right.y, right.z);
		glEnd();
	}
	

	bones[0].SetParent(nullptr);
	trans.SetParent(nullptr);
}

void Armature::Update(float deltaTime){
	time += deltaTime;
	currentBlendTime += deltaTime;

	if(currentBlendTime >= blendTime){
		currentAnimIndex = targetAnimIndex;
		time = currentBlendTime;
	}

	bool isBlending = currentAnimIndex != targetAnimIndex;

	for(int i = 0; i < boneCount; i++){
		Vector3 currentPos    = anim[currentAnimIndex].boneAnims[i].positionAnim.Evaluate(time);
		Quaternion currentRot = anim[currentAnimIndex].boneAnims[i].rotationAnim.Evaluate(time);

		if(!isBlending){
			bones[i].position = currentPos;
			bones[i].rotation = currentRot;
		}
		else{
			Vector3 targetPos    = anim[targetAnimIndex].boneAnims[i].positionAnim.Evaluate(currentBlendTime);
			Quaternion targetRot = anim[targetAnimIndex].boneAnims[i].rotationAnim.Evaluate(currentBlendTime);

			float blendWeight = currentBlendTime / blendTime;
			Vector3 blendedPos    = currentPos * (1 - blendWeight) + targetPos * blendWeight;
			Quaternion blendedRot = currentRot * (1 - blendWeight) + targetRot * blendWeight;

			bones[i].position = blendedPos;
			bones[i].rotation = blendedRot;
		}
	}
}

BoneTransform* Armature::GetBoneByName(const string& name){
	for(int i = 0; i < boneCount; i++){
		if(bones[i].name == name){
			return &bones[i];
		}
	}

	return nullptr;
}

int Armature::GetAnimationIndexByName(const string& name){
	for(int i = 0; i < animCount; i++){
		if(anim[i].stateName == name){
			return i;
		}
	}

	return -1;
}

void Armature::BlendTo(const string& animName, float time){
	int index = GetAnimationIndexByName(animName);
	if(index != -1){
		BlendToIndex(index, time);
	}
	else{
		cout << "\n\nError: trying to blend to invalid state: '" << animName << "'" << endl;
	}
}

void Armature::BlendToIndex(int animIndex, float time){
	if(targetAnimIndex != animIndex){
		targetAnimIndex = animIndex;
		blendTime = time;
		currentBlendTime = 0;
	}
}

void Armature::ParseStateString(const string& states){
	vector<string> stateList = SplitStringByDelimiter(states, ",");
	int stateCount = 1;
	for(const string& state : stateList){
		vector<string> parsedState = SplitStringByDelimiter(state, ":");
		if(parsedState.size() != 2){
			cout << "Warning: Invalid state description: '" << state << "'" << endl;
		}
		else{
			const string& stateName = parsedState[0];
			const string& stateTimes = parsedState[1];
			vector<string> stateTimesSplit = SplitStringByDelimiter(stateTimes, "-");
			if(stateTimesSplit.size() != 2){
				cout << "Warning: Invalid state times: '" << stateTimes << "'" << endl;
				break;
			}

			float timeMin = atof(stateTimesSplit[0].c_str());
			float timeMax = atof(stateTimesSplit[1].c_str());

			anim[stateCount].stateName = stateName;
			for(int i = 0; i < boneCount; i++){
				anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(anim[0].boneAnims[i].positionAnim.Evaluate(timeMin), timeMin);
				anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(anim[0].boneAnims[i].positionAnim.Evaluate(timeMax), timeMax);
				for(KeyFrame<Vector3> frame : anim[0].boneAnims[i].positionAnim.keyFrames){
					if(frame.time > timeMin && frame.time < timeMax){
						anim[stateCount].boneAnims[i].positionAnim.AddKeyFrame(frame.value, frame.time);
					}
				}

				anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(anim[0].boneAnims[i].rotationAnim.Evaluate(timeMin), timeMin);
				anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(anim[0].boneAnims[i].rotationAnim.Evaluate(timeMax), timeMax);
				for(KeyFrame<Quaternion> frame : anim[0].boneAnims[i].rotationAnim.keyFrames){
					if(frame.time > timeMin && frame.time < timeMax){
						anim[stateCount].boneAnims[i].rotationAnim.AddKeyFrame(frame.value, frame.time);
					}
				}
			}

			stateCount++;
		}
	}

	animCount = stateCount;
}

string Armature::EncodeStateString(){
	string encoded="";
	bool first = true;
	for(int i = 1; i < animCount; i++){
		if(!first){
			encoded += ",";
		}

		encoded += (anim[i].stateName + ":");

		float start = anim[i].boneAnims[0].positionAnim.keyFrames[0].time;
		float end = start + anim[i].boneAnims[0].positionAnim.Length();

		encoded += (to_string(start) + "-" + to_string(end));
		first = false;
	}

	return encoded;
}

