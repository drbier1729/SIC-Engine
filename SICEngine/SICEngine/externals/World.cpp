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

#include "World.h"

#include "../Components.h"
#include "../System.h"
#include "../Events.h"
#include "../EventManager.h"
#include "../Entities.h"


using std::pmr::vector;
using std::pmr::map;
using std::pair;

typedef map<ArbiterKey, Arbiter>::iterator ArbIter;
typedef pair<ArbiterKey, Arbiter> ArbPair;

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

void World::Add(sic::Body* body)
{
	bodies.push_back(body);
}

void World::Add(sic::Entity const& entity)
{
	assert(entity.Has<sic::Body>());
	sic::Body* body = &entity.Get<sic::Body>();
	
	assert(entity.Has<sic::Transform>());
	sic::Transform const& tr = entity.Get<sic::Transform>();

	body->UpdatePosAndRot(sic::vec2(tr.position.xy), tr.rotation.z);
	
	if (!body->setWidthManually) { body->width = sic::vec2(tr.scale.xy); }

	bodies.push_back(body);
}

//void World::Add(sic::Joint* joint)
//{
//	joints.push_back(joint);
//}

void World::Clear()
{
	bodies.clear();
	joints.clear();
	arbiters.clear();
}

void World::BroadPhase()
{
	// O(n^2) broad-phase
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		sic::Body* bi = bodies[i];

		for (int j = i + 1; j < (int)bodies.size(); ++j)
		{
			sic::Body* bj = bodies[j];

			if (bi->invMass == 0.0f && bj->invMass == 0.0f)
				continue;

			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);
			ArbIter iter = arbiters.find(key);

			sic::Entity entity1 = sic::System::ECS().GetOwner(*newArb.body1);
			sic::Entity entity2 = sic::System::ECS().GetOwner(*newArb.body2);

			if (newArb.numContacts > 0)
			{
				if (iter == arbiters.end())
				{
					arbiters.insert(ArbPair(key, newArb));

					// event for collision enter
					sic::System::Events().EnqueueEvent(sic::EventType::CollisionEnter, 0.0f, {
						{"EntityID_primary", entity1},
						{"EntityID_secondary", entity2}
					});
				}
				else
				{
					iter->second.Update(newArb.contacts, newArb.numContacts);
				}

				if (entity2.Has<sic::GroundTag>()) {
					for (sic::i32 k = 0; k < newArb.numContacts; ++k) {
						if (glm::dot(newArb.contacts[k].normal, sic::vec2(0.0f, -1.0f)) > 0.5f) {

							newArb.body1->isGrounded = true;
							break;
						}
					}
				}

				if (entity1.Has<sic::GroundTag>()) {
					for (sic::i32 k = 0; k < newArb.numContacts; ++k) {
						if (glm::dot(newArb.contacts[k].normal, sic::vec2(0.0f, 1.0f)) > 0.5f) {

							newArb.body2->isGrounded = true;
							break;
						}
					}
				}
			}
			else if (iter != arbiters.end()) {
				arbiters.erase(iter);

				// event for collision exit
				sic::System::Events().EnqueueEvent(sic::EventType::CollisionExit, 0.0f, {
					{"EntityID_primary", entity1},
					{"EntityID_secondary", entity2}
				});

				if (entity2.Has<sic::GroundTag>()) {
					newArb.body1->isGrounded = false;
				}

				if (entity1.Has<sic::GroundTag>()) {
					newArb.body2->isGrounded = false;
				}
			}
		}
	}
}

void World::Remove(sic::Body* body)
{
	auto it = std::find(bodies.begin(), bodies.end(), body);
	if (it != bodies.end()) { bodies.erase(it); }
}

void World::Remove(sic::Entity const& entity) {
	sic::Body* body = &entity.Get<sic::Body>();
	auto it = std::find(bodies.begin(), bodies.end(), body);
	if (it != bodies.end()) { bodies.erase(it); }
}

//void World::Remove(sic::Joint* joint)
//{
//	auto it = std::find(joints.begin(), joints.end(), joint);
//	if (it != joints.end()) { joints.erase(it); }
//}

void World::Step(float dt)
{
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

	// Determine overlapping bodies and update contact points.
	BroadPhase();

	// Integrate forces.
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		sic::Body* b = bodies[i];

		if (b->isTrigger == false)
		{
			if (b->invMass != 0.0f)
			{
				b->velocity += dt * (gravity + b->invMass * b->force);
				b->velocity *= 0.99f;

				if (b->velocity.x > 0.01f) {
					b->forward = true;
				}
				else if (b->velocity.x < -0.01f) {
					b->forward = false;
				}
			}
			if (b->I != 0.0f && b->enableRotation)
			{
				b->angularVelocity += dt * b->invI * b->torque;
				b->angularVelocity *= 0.99f;
			}
		}
	}

	// Perform pre-steps.
	for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb)
	{
		arb->second.PreStep(inv_dt);
	}

	// // TODO: joints not yet supported
	//for (int i = 0; i < (int)joints.size(); ++i)
	//{
	//	joints[i]->PreStep(inv_dt);	
	//}

	// Perform iterations
	for (int i = 0; i < iterations; ++i)
	{
		for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb)
		{
			arb->second.ApplyImpulse();
		}


		// // TODO: joints not yet supported
		//for (int j = 0; j < (int)joints.size(); ++j)
		//{
		//	joints[j]->ApplyImpulse();
		//}
	}

	// Integrate Velocities
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		sic::Body* b = bodies[i];
		
		if (b->isTrigger == false)
		{
			b->position += dt * b->velocity;
			if (b->enableRotation)
			{
				b->rotation += dt * b->angularVelocity;
			}

			b->force = sic::vec2(0.0f, 0.0f);
			b->torque = 0.0f;
		}

	}
}
