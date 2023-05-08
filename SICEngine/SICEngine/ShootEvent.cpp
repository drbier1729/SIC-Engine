/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include "Events.h"

#include "SICEngine.h"

/*
	Shoot Args
	"EntityID_player"			: EntityID
*/

namespace sic {

	void ShootBall(Event const& e)
	{
		EntityID p_id = e.GetArg("EntityID_player"_sid).AsEntityID();
		Entity p = System::ECS().GetEntityFromID(p_id);

		Entity projectile = System::SceneFactory().BuildEntityFromArchetype("Projectile.json"_sid);

		auto& proj_tr = projectile.Get<Transform>();
		proj_tr.position = p.Get<Transform>().position;

		//auto& proj_rb = projectile.Get<RigidBody>();
		//proj_rb.velocity = vec4(30.0f, 0.0, 0.0, 0.0f); // TODO : this should be aimed by mouse or joystick

		auto& proj_body = projectile.Get<Body>();
		proj_body.velocity = vec2(30.0f, 0.0); // TODO : this should be aimed by mouse or joystick

		auto& proj_info = projectile.Get<Projectile>();
		proj_info.spawnedBy = p.Id();

		System::Events().EnqueueEvent(EventType::Destroy, 5.0f, {
			{"EntityID", projectile}
		});
	}

}