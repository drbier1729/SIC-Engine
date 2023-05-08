/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: -
 */
 
#include "ComponentSystems.h"

#include "SICEngine.h"

namespace sic {

	void PatrolSystem(Patrol& p)
	{
		Entity e = sic::System::ECS().GetOwner(p);

		f32 range = p.range;
		f32 distance = 0.0f;
		f32 speed = p.speed;
		vec4 direction(0.0f);
		vec4 player_pos;

		bool IsHiding = false;


		if (e.Has<Transform>()/* && e.Has<RigidBody>()*/ && e.Has<Body>()) {
			//auto& rb = e.Get<RigidBody>();
			auto& body = e.Get<Body>();
			auto& t = e.Get<Transform>();

			// object needs to be on ground to start patrol
			if (!body.isGrounded) return;

			// detecting part
			System::ECS().ForEach<PlayerController>([&player_pos, &IsHiding](PlayerController& pc) -> void
				{
					// get player's position
					Entity player_entity = System::ECS().GetOwner(pc);
					if (player_entity.Has<Transform>()) { player_pos = player_entity.Get<Transform>().position; }

					// get player IsHiding
					IsHiding = pc.is_hiding;
				});

			// check if player is in the object's eye sight
			vec3 PtoObject = glm::normalize(vec3(player_pos.x - t.position.x, 0.0f, 0.0f));

			// object is not looking at the player's direction
			if ((PtoObject.x < 0.0f && p.direction.x > 0.0f) || (PtoObject.x > 0.0f && p.direction.x < 0.0f)) {
				p.isDetected = false;
			}
			else if (IsHiding) {
				// player is hiding so object can not detect the player
				p.isDetected = false;
			}
			else {
				// get distance between object and player
				distance = glm::distance(player_pos, t.position);

				// check if player is in the radius
				p.isDetected = distance <= p.radius;
			}

			// mode = sentinel mode. stays in the range
			if (p.isDetected)
				range = p.mode ? p.range : 1000.0f;
			else
				range = p.mode ? 0.0f : p.range;

			// object is out of patrol range 
			if (t.position.x > p.startingPoint.x + range || t.position.x < p.startingPoint.x - range) {
				// get direction to the other point
				glm::vec3 d = glm::vec3(p.startingPoint.x - t.position.x, 0.0f, 0.0f);
				d = glm::normalize(d);

				// if it's direction is not facing the other point, switch it.
				if (!glm::all(glm::equal(d, glm::normalize(p.direction))))
					p.direction = -p.direction;
			}


			// get object speed up
			if (p.isDetected) {
				speed = (2 - fabsf(t.position.x - player_pos.x) / range) * p.speed * p.acceleration;
			}
			else if (!p.isDetected && p.mode) {
				if (fabsf(t.position.x - p.startingPoint.x) < 1.0f)
					speed = 0.1f;
			}

			// apply moving speed to velocity
			direction = vec4(p.direction * speed, 0.0f);
			//rb.velocity += direction; // Old
			body.velocity += vec2(direction.xy); // New
		}
	}

}