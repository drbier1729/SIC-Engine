/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#include "ComponentSystems.h"

#include "SICEngine.h"
#include <gtx/norm.hpp>

namespace sic {
	
	void DestroyOutOfBoundsSystem(Transform& tr)
	{
		Entity owner = System::ECS().GetOwner(tr);
		if (owner.IsValid())
		{
			if (glm::distance2(tr.position, vec4(0, 0, 0, 0)) > 1000000.0f)
			{
				System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
					{"EntityID", owner}
				});
			}
		}
	}
}