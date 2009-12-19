/*
   Copyright (C) 2009 Sony Computer Entertainment Inc.
   All rights reserved.

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

#ifndef __CAPSULESPHEREDISTANCE_H__
#define __CAPSULESPHEREDISTANCE_H__

#include "Capsule.h"
#include "Sphere.h"

//---------------------------------------------------------------------------
// capsuleSphereDistance:
//
// returns:
//    positive or negative distance between capsule and sphere.
//
// args:
//    Vector3& normal: set to a unit contact normal pointing from capsule (A) to sphere (B).
//
//    CapsulePoint& capsulePointA, SpherePoint& spherePointB:
//       set to a closest point or point of penetration on each object
//
//    const Capsule& capsuleA, const Sphere& sphereB: object sizes.
//
//    const Transform3 & transformA: column 0 gives the direction of the capsule;
//                                    the translation gives the center position
//    const Transform3 & transformB: the translation gives the center position
//
//    float distanceThreshold:
//       the algorithm will exit early if it finds that the objects are more distant than this
//       threshold, and not compute a contact normal or points.  if this distance returned
//       exceeds the threshold, all the other output data may not have been computed.  by
//       default, this is set to MAX_FLOAT so it will have no effect.
//
//---------------------------------------------------------------------------

float
capsuleSphereDistance(
	Vector3 & normal,
	CapsulePoint & capsulePointA,
	SpherePoint & spherePointB,
	const Capsule & capsuleA, const Transform3 & transformA,
	Sphere sphereB, const Transform3 & transformB,
	float distanceThreshold = FLT_MAX );

#endif /* __CAPSULESPHEREDISTANCE_H__ */
