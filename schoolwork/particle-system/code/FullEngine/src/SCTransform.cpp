#include "../header/int/SCTransform.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"

SC_Transform::SC_Transform(){
	position = Vector3(0,0,0);
	rotation = QUAT_IDENTITY;
	scale = Vector3(1,1,1);
	
	gameObject = NULL;
	parent = NULL;
}

SC_Transform::SC_Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, SC_Transform* _parent){
	position  = _position;
	rotation  = _rotation;
	scale     = _scale;
	
	SetParent(_parent);

	gameObject = NULL;
}

Vector3 SC_Transform::Forward() const{
	return (LocalToGlobal(Vector3(0,0,1)) - LocalToGlobal(Vector3(0,0,0))).Normalized();
	/*
	if(parent != NULL){
		return Rotate(parent->Forward(), rotation);
	}
	return Rotate(Z_AXIS, rotation);
	*/
}

Vector3 SC_Transform::Up() const{
	return (LocalToGlobal(Vector3(0,1,0)) - LocalToGlobal(Vector3(0,0,0))).Normalized();
	/*
	if(parent != NULL){
		return Rotate(parent->Up(), rotation);
	}
	return Rotate(Y_AXIS, rotation);
	*/
}

Vector3 SC_Transform::Right() const{
	return (LocalToGlobal(Vector3(1,0,0)) - LocalToGlobal(Vector3(0,0,0))).Normalized();
	/*
	if(parent != NULL){
		return Rotate(parent->Right(), rotation);
	}
	return Rotate(X_AXIS, rotation);
	*/
}

void SC_Transform::SetParent(SC_Transform* _parent){
	if(parent != NULL){
		for(auto iter = parent->children.begin(); iter != parent->children.end(); iter++){
			if(*iter == this){
				parent->children.erase(iter);
				break;
			}
		}
	}

	if(_parent == this){
		cout << "\n\nWarning: Trying to set a transform's parent to itself\n";
		int x = *((int*)0);
		return;
	}

	parent = _parent;

	if(parent == NULL){
		return;
	}

	bool alreadyThere = false;
	for(auto iter = parent->children.begin(); iter != parent->children.end(); iter++){
		if(*iter == this){
			alreadyThere = true;
			break;
		}
	}

	if(!alreadyThere){
		parent->children.push_back(this);
	}
}

SC_Transform* SC_Transform::GetParent() const{
	return parent;
}

Mat4x4 SC_Transform::LocalToGlobalMatrix() const{
	if(parent == this){
		cout << "\n\nWarning: Trying to set a transform's parent to itself\n";
		int x = *((int*)0);
	}

	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS * scale.x, rotation), 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS * scale.y, rotation), 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS * scale.z, rotation), 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = affMat * linMat;

	if(parent != NULL){
		return parent->LocalToGlobalMatrix() * transMat;
	}

	return transMat;
}

Mat4x4 SC_Transform::GlobalToLocalMatrix() const{
	if(parent == this){
		cout << "\n\nWarning: Trying to set a transform's parent to itself\n";
		int x = *((int*)0);
	}

	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS, rotation.Conjugate()) / scale.x, 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS, rotation.Conjugate()) / scale.y, 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS, rotation.Conjugate()) / scale.z, 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, -position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, -position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, -position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = linMat * affMat;

	if(parent != NULL){
		return transMat * parent->GlobalToLocalMatrix();
	}

	return transMat;
}

Mat4x4 SC_Transform::GetCameraMatrix() const{
	Mat4x4 transMat;

	Mat4x4 linMat;
	Mat4x4 affMat;

	linMat.SetColumn(0, Vector4(Rotate(X_AXIS / scale.x, rotation.Conjugate()), 0));
	linMat.SetColumn(1, Vector4(Rotate(Y_AXIS / scale.y, rotation.Conjugate()), 0));
	linMat.SetColumn(2, Vector4(Rotate(Z_AXIS / scale.z, rotation.Conjugate()), 0));
	linMat.SetColumn(3, Vector4(0,0,0,1));

	affMat.SetRow(0, Vector4(X_AXIS, -position.x));
	affMat.SetRow(1, Vector4(Y_AXIS, -position.y));
	affMat.SetRow(2, Vector4(Z_AXIS, -position.z));
	affMat.SetRow(3, Vector4(0,0,0,1));

	transMat = linMat * affMat;

	if(parent != NULL){
		return transMat * parent->GetCameraMatrix();
	}

	return transMat;
}

SC_Transform SC_Transform::GetInverse() const{
	return SC_Transform(position*-1, 
						rotation.Conjugate(), 
						Vector3(1/scale.x, 1/scale.y, 1/scale.z));
} 

Vector3 SC_Transform::GlobalPosition() const{
	if(parent == NULL){
		return position;
	}
	else{
		return parent->LocalToGlobal(position);
	}
}

Quaternion SC_Transform::TotalRotation() const{

	if(parent == NULL){
		return rotation;
	}

	return parent->TotalRotation() * rotation;
}

Vector3 SC_Transform::TotalScale() const{
	if(parent == NULL){
		return scale;
	}

	return scale.Scaled(parent->TotalScale());
}

Vector3 SC_Transform::GlobalToLocal(const Vector3& global) const{
	Vector3 localVec = global;
	if(parent != NULL){
		localVec = parent->GlobalToLocal(global);
	}
    localVec = localVec - position;
    localVec = Rotate(localVec, rotation.Conjugate());
    localVec = localVec.Scaled(Vector3( 1 / scale.x,
                                        1 / scale.y,
                                        1 / scale.z));

    return localVec;
}

Vector3 SC_Transform::LocalToGlobal(const Vector3& local) const{
	if(parent == this){
		cout << "\n\nWarning: Trying to set a transform's parent to itself\n";
		int x = *((int*)0);
	}

    Vector3 globalVec = local;

	globalVec = globalVec.Scaled(Vector3(1/scale.x, 1/scale.y, 1/scale.z));
	globalVec = Rotate(globalVec, rotation);
	globalVec = globalVec + position;

	if(parent != NULL){
		return parent->LocalToGlobal(globalVec);
	}

	return globalVec; 

    globalVec = globalVec.Scaled(scale);
    globalVec = Rotate(globalVec, rotation);
    globalVec = globalVec + position;

    return globalVec;
}

