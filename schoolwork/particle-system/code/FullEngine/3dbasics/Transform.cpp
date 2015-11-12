/*
This is the Transform source file for the 3dbasics program, which provides simple tools
to represent and compute a 3D environment.

Author: Benji Smith, 2014
Licensed under the GNU GPL v3
Full License Text found in LICENSE file
*/

#include "Transform.h"

Transform::Transform(){
    position = Vector3();
    rotation = Quaternion();
    scale    = Vector3(1,1,1);
}

Transform::Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale){
    position = _position;
    rotation = _rotation;
    scale    = _scale;
}

//NOTE: does not take into account transform.position, since that's an affine transformation.
Basis3D Transform::GetBasis() const{
    return (Basis3D( Rotate(X_AXIS * scale.x, rotation),
                     Rotate(Y_AXIS * scale.y, rotation),
                     Rotate(Z_AXIS * scale.z, rotation)));
}

//Gets the matrix that represents the transformations of the Transform.
//NOTE: Does not take into account transform.position, since that's an affine transformation
Matrix3x3 Transform::GetMatrix() const{
    return MakeMatrixFromColumns( Rotate(X_AXIS * scale.x, rotation),
                                  Rotate(Y_AXIS * scale.y, rotation),
                                  Rotate(Z_AXIS * scale.z, rotation));
}

Vector3 Transform::Forward() const{
	return Rotate(Z_AXIS, rotation);
}

Vector3 Transform::GlobalToLocal(const Vector3& global) const{
    Vector3 localVec;
    localVec = global - position;
    localVec = Rotate(localVec, rotation.Conjugate());
    localVec = localVec.Scaled(Vector3( 1 / scale.x,
                                        1 / scale.y,
                                        1 / scale.z));

    return localVec;
}

Vector3 Transform::LocalToGlobal(const Vector3& local) const{
    Vector3 globalVec;
    globalVec = local.Scaled(scale);
    globalVec = Rotate(globalVec, rotation);
    globalVec = globalVec + position;

    return globalVec;
}


