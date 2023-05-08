/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "Events.h"

#include "SICEngine.h"

/*
	WeaponFired Args
	"EntityID_player"			: EntityID
	"f32_mouseX"				: f32
	"f32_mouseY"				: f32
*/

namespace sic {

	void ThrowBall(Event const& e)
	{

		EntityID p_id = e.GetArg("EntityID_player"_sid).AsEntityID();
		f32 mouseX = e.GetArg("f32_mouseX").AsF32();
		f32 mouseY = e.GetArg("f32_mouseY").AsF32();

		Entity p = System::ECS().GetEntityFromID(p_id);

		Entity ball = System::SceneFactory().BuildEntityFromArchetype("BouncyBall.json");

		//assert(ball.Has<SpawnedBy>() && "Ball should have SpawnedBy.");
		//SpawnedBy& sb = ball.Get<SpawnedBy>();
		//sb.spawned_by = p.Id();

		Transform& tr = p.Get<Transform>();
		PlayerController& pc = p.Get<PlayerController>();

		assert(ball.Has<Transform>() && "Ball should have Transform.");
		Transform& tr_ball = ball.Get<Transform>();
		tr_ball.position = tr.position;
		tr_ball.rotation = tr.rotation;

		System::Events().EnqueueEvent(EventType::Destroy, 5.0f, {
			{"EntityID", ball}
		});

		vec2 mouse = vec2{ 0.0f };
		mouse.x = mouseX;
		mouse.y = mouseY;

		vec2 move = vec2{ 0.0f };
		move = mouse - tr_ball.position.xy;
		move = glm::normalize(move);
		move *= 40.0f;

		//assert(ball.Has<RigidBody>() && "Ball should have RigidBody."); // Old
		assert(ball.Has<Body>() && "Ball should have Body.");
		//RigidBody& rb_ball = ball.Get<RigidBody>(); // Old
		Body& b_ball = ball.Get<Body>();
		move *= pc.acceleration * System::FrameRate().DeltaTime();
		b_ball.velocity += move;
	}

}