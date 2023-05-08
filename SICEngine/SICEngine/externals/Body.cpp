/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability 
* of this software for any purpose.  
* It is provided "as is" without express or implied warranty.
*/

// Source: https://github.com/erincatto/box2d-lite.

// Modifications to the original software (Box2D Lite):
/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier
 */

#include "../Components.h"
#include "../System.h"
#include "../Entities.h"

namespace sic {

	Body::Body() :
		position{ vec2{0.0f, 0.0f} },
		positionOffset{ vec2{0.0f, 0.0f} },
		rotation{ 0.0f },
		velocity{ vec2{0.0f, 0.0f} },
		angularVelocity{ 0.0f },
		force{ vec2{0.0f, 0.0f} },
		torque{ 0.0f },
		friction{ 0.2f },
		width{ vec2{0.0f, 0.0f} },
		forward{ true },
		mass{ FLT_MAX },
		invMass{ 0.0f },
		I{ FLT_MAX },
		invI{ 0.0f },
		restitutionCoeff{ 0.8f },
		isTrigger{ false },
		isGrounded{ false },
		enableRotation{ true },
		setWidthManually{ false }
	{ }

	void Body::CalculateImpulse()
	{
		if (mass < FLT_MAX)
		{
			invMass = 1.0f / mass;
			I = mass * (width.x * width.x + width.y * width.y) / 12.0f;
			invI = 1.0f / I;
		}
		else
		{
			invMass = 0.0f;
			I = FLT_MAX;
			invI = 0.0f;
		}
	}

	void Body::UpdatePosAndRot(vec2 const& pos, f32 rot) {
		position = pos;
		rotation = rot;
	}
}
