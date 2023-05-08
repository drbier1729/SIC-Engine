/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#include "Events.h"

#include "SICEngine.h"

/*
	Destroy Args
	"EntityID"				: EntityID
*/

namespace sic {

	void DestroyEntities(Event const& e) {
		EntityID e_id = e.GetArg("EntityID"_sid).AsEntityID();

		if (System::ECS().IsValidEntity(e_id))
		{
			Entity e = System::ECS().GetEntityFromID(e_id);
			if (e.Has<Body>())
			{
				Body* body = &e.Get<Body>();
				System::Physics().Remove(body);
			}

			/* TODO : joints not yet supported
			if (owner.Has<Joint>())
			{
				Joint* joint = &owner.Get<Joint>();
				System::Physics().Remove(joint);
			}
			*/

			System::ECS().DestroyEntity(e);
		}
	}

}