/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

namespace sic {
	template<ValidComponent C>
	EventArg::EventArg(C const& c)
		: type{ ArgType::COMPONENT_ID }, asU64{ C::id }
	{ }
}