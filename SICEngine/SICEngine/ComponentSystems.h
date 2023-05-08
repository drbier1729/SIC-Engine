/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include "Components.h"

#include "System.h"
#include "Events.h"
#include "EventManager.h"

namespace sic {

#define REGISTER_COMPONENT_SYSTEM(name, component_type) void name##(component_type &);
#define REGISTER_COMPONENT_SYSTEM_DT(name, component_type) void name##(component_type &, sic::f32);
#include "ComponentSystems.def"
#undef REGISTER_COMPONENT_SYSTEM
#undef REGISTER_COMPONENT_SYSTEM_DT


	template<ValidComponent C>
	void DestroyEntityWith(C& comp)
	{
		Entity owner = System::ECS().GetOwner(comp);
		System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
			{ "EntityID", owner }
		});
	}

}