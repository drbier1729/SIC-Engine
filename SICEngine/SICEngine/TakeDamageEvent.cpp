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
	TakeDamage Args
	"EntityID_e"				: EntityID
	"i32_damage"				: i32
*/

namespace sic {

	void CalculateDamage(Event const& e)
	{
		EntityID entity_id = e.GetArg("EntityID_e"_sid).AsEntityID();
		if (System::ECS().IsValidEntity(entity_id))
		{
			Entity entity = System::ECS().GetEntityFromID(entity_id);

			assert(entity.Has<Health>());
			auto& health = entity.Get<Health>();

			health.hp -= e.GetArg("i32_damage"_sid).AsI32();
			if (health.hp <= 0)
			{
				System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
					{"EntityID", entity}
				});
			}

		}
	}

}