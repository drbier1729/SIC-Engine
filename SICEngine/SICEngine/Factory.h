/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <unordered_map>
#include <functional>
#include <array>

#include <document.h> //rapidjson

#include "SICTypes.h"
#include "StringId.h"
#include "MemResources.h"
#include "Entities.h"

#include "System.h"
#include "ResourceManager.h"
#include "Components.h"
#include "ComponentSerialization.h"

namespace sic {

	using ComponentDeserializer = std::function<void(rapidjson::Value const&, Entity const&)>;

	class Factory
	{		
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;

		Factory(EntityAdmin& ecs);
		explicit Factory(EntityAdmin& ecs, allocator_type alloc);
		~Factory() = default;

		void BuildScene(StringId::underlying_type filePath_sid);
		Entity BuildEntityFromArchetype(StringId::underlying_type filePath_sid);
		
		void SaveEntityAsArchetype(StringId::underlying_type archetype_filePath_sid, Entity const& entity);
		void SaveEntityInstanceInScene(StringId::underlying_type scene_filePath_sid, Entity const& entity);
		void SaveScenePretty(StringId::underlying_type filePath_sid);


		// TODO : implement
		void SaveScene(StringId::underlying_type filePath_sid);

		// TODO : temporary for testing -- remove when done
		template<ValidComponent C>
		void SaveComponentOverrideToScene(StringId::underlying_type scene_filePath_sid,
			const char* entity_name, const char* component_name, C& component);

		
		// Overloads
		inline
		Entity BuildEntityFromArchetype(const char* filePath)  { StringId sid{ filePath }; return BuildEntityFromArchetype(sid.Id()); }

		inline
		Entity BuildEntityFromArchetype(StringId filePath_sid) { return BuildEntityFromArchetype(filePath_sid.Id()); }

		inline
		void BuildScene(const char* filePath)  { StringId sid{ filePath }; BuildScene(sid.Id()); }

		inline
		void BuildScene(StringId filePath_sid) { BuildScene(filePath_sid.Id()); }

	public:
		Factory(Factory const& src) = delete;
		Factory(Factory const& src, allocator_type alloc = {}) = delete;
		Factory(Factory&& src) = delete;
		Factory(Factory&& src, allocator_type alloc) = delete;
		Factory& operator=(Factory const& rhs) = delete;
		Factory& operator=(Factory&& rhs) = delete;
	
	private:
		void SaveEntityInstanceInScene(rapidjson::Document& scene_json, Entity const& entity);
	
	private:
		EntityAdmin& m_ecs_ref;

		// Map: "componentName"_sid -> ComponentDeserializer
		std::pmr::unordered_map<StringId::underlying_type, ComponentDeserializer> m_deserializers;
	};
}

namespace sic {


	// TODO : I think this can be deleted
	template<ValidComponent C>
	void Factory::SaveComponentOverrideToScene(StringId::underlying_type scene_filePath_sid,
		const char* entity_name, const char* component_name, C& component)
	{
		auto& json = System::Resources().GetJson(scene_filePath_sid);

		auto it = json.FindMember(entity_name);
		assert(it != json.MemberEnd());

		auto ovr_it = it->value.FindMember("Overrides");
		assert(ovr_it != it->value.MemberEnd());

		auto cmp_it = ovr_it->value.FindMember(component_name);
		if (cmp_it == ovr_it->value.MemberEnd())
		{
			rapidjson::Value obj;
			obj.SetObject();
			cmp_it->value.AddMember(rapidjson::Value(component_name, json.GetAllocator()).Move(),
									obj, 
									json.GetAllocator());
			cmp_it = ovr_it->value.FindMember(component_name);
		}

		Serialize<C>(cmp_it->value, component, json.GetAllocator());
	}
}

