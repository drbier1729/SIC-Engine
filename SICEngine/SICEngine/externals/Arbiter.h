/*
* Copyright (c) 2006-2009 Erin Catto http://www.gphysics.com
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
 * Primary Author: -
 * Additional Authors: -
 */

#ifndef ARBITER_H
#define ARBITER_H

#include "../SICTypes.h"

namespace sic {
	struct Body;
}

union FeaturePair
{
	struct Edges
	{
		char inEdge1;
		char outEdge1;
		char inEdge2;
		char outEdge2;
	} e;
	int value;
};

struct Contact
{
	Contact() : Pn(0.0f), Pt(0.0f), Pnb(0.0f) {}

	sic::vec2 position;
	sic::vec2 normal;
	sic::vec2 r1, r2;
	float separation;
	float Pn;	// accumulated normal impulse
	float Pt;	// accumulated tangent impulse
	float Pnb;	// accumulated normal impulse for position bias
	float massNormal, massTangent;
	float bias;
	FeaturePair feature;
};

struct ArbiterKey
{
	ArbiterKey(sic::Body* b1, sic::Body* b2)
	{
		if (b1 < b2)
		{
			body1 = b1; body2 = b2;
		}
		else
		{
			body1 = b2; body2 = b1;
		}
	}

	sic::Body* body1;
	sic::Body* body2;
};

struct Arbiter
{
	enum {MAX_POINTS = 2};

	Arbiter(sic::Body* b1, sic::Body* b2);

	void Update(Contact* contacts, int numContacts);

	void PreStep(float inv_dt);
	void ApplyImpulse();

	Contact contacts[MAX_POINTS];
	int numContacts;

	sic::Body* body1;
	sic::Body* body2;

	// Combined friction
	float friction;
};

// This is used by std::set
inline bool operator < (const ArbiterKey& a1, const ArbiterKey& a2)
{
	if (a1.body1 < a2.body1)
		return true;

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
		return true;

	return false;
}

int Collide(Contact* contacts, sic::Body* body1, sic::Body* body2);

#endif
