/*
This is the Transform header file for the 3dbasics program, which provides simple tools
to represent and compute a 3D environment.

Author: Benji Smith, 2014
Licensed under the GNU GPL v3
Full License Text found in LICENSE file
*/

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"
#include "Basis.h"
#include "Matrix.h"

/*
The Transform structure holds information about the three main transformations for a 3D rigidbody: position, rotation, and scale.
Note that position is an affine transformation, so this structure is not equivalent to the Basis3D structure, which only holds linear transformations.
Also, the basis vectors that the Transform produces are always orthogonal (perpendicular to one another), since they're just rotated and scaled.
*/

struct Transform
{
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

    //Constructors
    Transform();
    Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale);

    //NOTE: does not take into account transform.position, since that's an affine transformation.
    Basis3D GetBasis() const;

    //Get's the matrix that represents the transformations of the Transform.
    //NOTE: Does not take into account transform.position, since that's an affine transformation
    Matrix3x3 GetMatrix() const;

	//Get the forward facing vector of the transform
	Vector3 Forward() const;
	
    //Transform a vector into this transform's space
    Vector3 GlobalToLocal(const Vector3& original) const;

    //Transform a vector from this transform's space
    Vector3 LocalToGlobal(const Vector3& original) const;
};

#endif // TRANSFORM_H
