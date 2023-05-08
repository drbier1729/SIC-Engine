/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"

namespace sic {

	enum class EventType : EventID
	{
#define REGISTER_EVENT(type) type,
#define REGISTER_HANDLER(name)
#include "Events.def" 
#undef REGISTER_HANDLER
#undef REGISTER_EVENT
		COUNT
	};

	constexpr size_t NUM_EVENTS{ static_cast<size_t>(EventType::COUNT) };
}

namespace sic {

	class Event;

#define REGISTER_EVENT(type)
#define REGISTER_HANDLER(name) void name##(Event const&);
#include "Events.def"
#undef REGISTER_HANDLER
#undef REGISTER_EVENT
}