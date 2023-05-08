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
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier
 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>

#include "MathUtils.h"
#include "Arbiter.h"

namespace sic {
	struct Body;
	struct Joint;
	struct Entity;
}

struct World
{
	using allocator_type = std::pmr::polymorphic_allocator<>;

	World(sic::vec2 gravity, int iterations) : World(gravity, iterations, allocator_type()) {}
	explicit World(sic::vec2 gravity, int iterations, allocator_type alloc) 
		: bodies(alloc), joints(alloc), arbiters(alloc),
		gravity(gravity), iterations(iterations) 
	{}

	void Add(sic::Body* body);
	void Add(sic::Entity const& entity);
	void Remove(sic::Body* body);
	void Remove(sic::Entity const& entity);

	// TODO : Joints are not yet fully supported. Have to work out some things about lifetime.
	// This might be as simple as adding a loop to clear out invalidated joints in the CleanUpCorpses system.
	// However, serialization and adding/removing joints on the fly still raise some questions.
	//void Add(sic::Joint* joint);
	//void Remove(sic::Joint* joint);

	void Clear();

	void Step(float dt);

	void BroadPhase();

	// TODO : this could just store a ptr to the ECS and operate on the components directly
	std::pmr::vector<sic::Body*> bodies;
	std::pmr::vector<sic::Joint*> joints;

	std::pmr::map<ArbiterKey, Arbiter> arbiters;
	sic::vec2 gravity;
	int iterations;
	static bool accumulateImpulses;
	static bool warmStarting;
	static bool positionCorrection;
};

#endif
