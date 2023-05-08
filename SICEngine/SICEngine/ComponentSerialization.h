/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Andrew Rudasics, Aseem Apastamb
 */
 

#pragma once

#include "Components.h"
#include "Entities.h"
#include "Reflector.h"
#include <document.h>

namespace sic {

	template<ValidComponent C>
	void Serialize(rapidjson::Value& val, C const& comp, rapidjson::MemoryPoolAllocator<>& alloc);

	template<ValidComponent C>
	void Deserialize(rapidjson::Value const& val, Entity const& e);
	
	template<ValidComponent C>
	rapidjson::Value& SerializeReflectable(rapidjson::Value& val, C const& comp, rapidjson::MemoryPoolAllocator<>& alloc);

	template<ValidComponent C>
	C& DeserializeReflectable(rapidjson::Value const& val, Entity const& e);

}

namespace sic {
	// Specializations

	template<>
	void Serialize<SpriteRenderer>(rapidjson::Value& val, SpriteRenderer const& sr, rapidjson::MemoryPoolAllocator<>& alloc);

	template<>
	void Serialize<SpriteAnimator>(rapidjson::Value& val, SpriteAnimator const& sa, rapidjson::MemoryPoolAllocator<>& alloc);
	
	template<>
	void Serialize<Joint>(rapidjson::Value& val, Joint const& jnt, rapidjson::MemoryPoolAllocator<>& alloc);
	

	template<>
	void Deserialize<SpriteRenderer>(rapidjson::Value const& val, Entity const& e);

	template<>
	void Deserialize<SpriteAnimator>(rapidjson::Value const& val, Entity const& e);
	
	template<>
	void Deserialize<Body>(rapidjson::Value const& val, Entity const& e);

	template<>
	void Deserialize<Joint>(rapidjson::Value const& val, Entity const& e);

	template<>
	void Deserialize<Patrol>(rapidjson::Value const& val, Entity const& e);

	template<>
	void Deserialize<Text>(rapidjson::Value const& val, Entity const& e);
}

#include "ComponentSerialization.tpp"