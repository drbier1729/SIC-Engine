/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include "Factory.h"

#include <document.h>
#include <filereadstream.h>

#include <charconv>

namespace sic {
	
	// =======
	// Factory
	// ======= 
	
	Factory::Factory(EntityAdmin& ecs)
		: Factory(ecs, allocator_type{})
	{ }


	Factory::Factory(EntityAdmin& ecs, allocator_type alloc)
		: m_ecs_ref{ ecs },
		m_deserializers(NUM_COMPONENTS, alloc)
	{ 
#define REGISTER_COMPONENT(type) m_deserializers.emplace(StrHash(#type), Deserialize<type>);
#include "Components.def"
#undef REGISTER_COMPONENT
	}

	void Factory::BuildScene(StringId::underlying_type filePath_sid)
	{
		rapidjson::Document const& doc{ System::Resources().GetJson(filePath_sid) };

		Entity e{ m_ecs_ref };
		rapidjson::Value::ConstMemberIterator ent_itr;
	
		for (auto const& m : doc.GetObject())
		{
			// Create Entity from Archetype, if provided
			ent_itr = m.value.FindMember("Archetype");
			if (ent_itr != m.value.MemberEnd())
			{
				e = BuildEntityFromArchetype(ent_itr->value.GetString());
			}
			else
			{
				e = m_ecs_ref.MakeEntity();
			}

			assert(e.IsValid());
			
			// Override existing component data
			ent_itr = m.value.FindMember("Overrides");
			if (ent_itr != m.value.MemberEnd())
			{
				for (auto const& ovr : ent_itr->value.GetObject())
				{
					assert("No component name given." && ovr.name.IsString());
					StringId compName{ ovr.name.GetString() };

					auto it = m_deserializers.find(compName.Id());
				
				#ifdef _DEBUG
					if (it == m_deserializers.end())
					{
						printf_s("Component specified in JSON doc \"%s\" does not exist: \"%s\"\n", System::Resources().GetFilepath(filePath_sid), compName.Str());
						assert(false);
					}
				#endif

					// Call deserializer
					it->second(ovr.value, e);
				}
			}
		}
	}
	
	Entity Factory::BuildEntityFromArchetype(StringId::underlying_type filePath_sid)
	{
		rapidjson::Document const& doc{ System::Resources().GetJson(filePath_sid) };
	
		Entity e{ m_ecs_ref };
		rapidjson::Value::ConstMemberIterator doc_itr;
	
		// If Entity has an Archetype, build it recursively based on that.
		doc_itr = doc.FindMember("Archetype");
		if (doc_itr != doc.MemberEnd())
		{
			e = BuildEntityFromArchetype(doc_itr->value.GetString());
		}
		else
		{
			e = m_ecs_ref.MakeEntity();
		}

		assert(e.IsValid());
	
		// Add or Override components
		doc_itr = doc.FindMember("Components");
		if (doc_itr != doc.MemberEnd())
		{
			for (auto const& cmp : doc_itr->value.GetObject())
			{
				assert("No component name given." && cmp.name.IsString());
				StringId compName{ cmp.name.GetString() };

				m_deserializers[compName.Id()](cmp.value, e);
			}
		}
		
		return e;
	}
	
	// TODO (dylan) : rethink this... do we reaalllly want to save EVERYTHING from the EntityAdmin?
	// If so, do we want to save queued Events as well? Any other state?
	// This should probably just be an "Editor" function that saves Component data only and 
	// should probably save to a human-readable format for now.
	void Factory::SaveScene(StringId::underlying_type filePath_sid)
	{
		// Look up JSON doc in RM
		// Clear doc
		// Serialize all data in EntityAdmin into doc
		// - id info (count, generations, reusable, etc)
		// - EntityTypes
		// - SparseTable
		// - DenseTable
		// Write doc to JSON file on disk (optional)


		// Note: if this is how our scene save files will look, we need a different BuildScene method.
		// 
		// Maybe we can have a schema for SceneInit files (which are human readable) and SceneSave files
		// (which just reflect the ECS state).
		// 
		// Alternatively, we can write a separate tool that converts human readable SceneInit files
		// into SceneSave files (maybe even just by building the scene in an instance of an EntityAdmin
		// then immediately serializing it).
	}

	void Factory::SaveScenePretty(StringId::underlying_type filePath_sid)
	{
		auto& json = System::Resources().GetJson(filePath_sid);
		json.RemoveAllMembers();

		m_ecs_ref.ForEachEntity([this, &json](Entity const& entity) -> void 
		{
			SaveEntityInstanceInScene(json, entity);
		});
	}

	void Factory::SaveEntityInstanceInScene(StringId::underlying_type scene_filePath_sid, Entity const& entity)
	{
		auto& json = System::Resources().GetJson(scene_filePath_sid);
		SaveEntityInstanceInScene(json, entity);
	}

	void Factory::SaveEntityAsArchetype(StringId::underlying_type archetype_filePath_sid, Entity const& entity)
	{
		auto& json = System::Resources().GetJson(archetype_filePath_sid);

		EntityID eid{ m_ecs_ref.GetIdFromEntity(entity.Id()) };

		rapidjson::Value compOverridesObj;
		compOverridesObj.SetObject();

		auto SerializeComponent = [&json, &compOverridesObj](ValidComponent auto const& component)
		{
			rapidjson::Value compObj;
			compObj.SetObject();

			Serialize(compObj, component, json.GetAllocator());

			compOverridesObj.AddMember(rapidjson::StringRef(ComponentNameFromId(component.id)),
				compObj, json.GetAllocator());
		};

		entity.ForEachComponent(SerializeComponent);

		auto doc_itr = json.FindMember("Components");
		if (doc_itr != json.MemberEnd())
		{
			doc_itr->value.Swap(compOverridesObj);
		}
		else
		{
			json.AddMember("Components", compOverridesObj, json.GetAllocator());
		}
	}


	// private helper. Assumes that entity is valid
	void Factory::SaveEntityInstanceInScene(rapidjson::Document& json, Entity const& entity)
	{
		// Component Overrides
		rapidjson::Value compOverridesObj;
		compOverridesObj.SetObject();

		auto SerializeComponent = [&json, &compOverridesObj](ValidComponent auto const& component)
		{
			rapidjson::Value compObj;
			compObj.SetObject();

			Serialize(compObj, component, json.GetAllocator());

			compOverridesObj.AddMember(rapidjson::StringRef(ComponentNameFromId(component.id)),
				compObj, json.GetAllocator());
		};

		entity.ForEachComponent(SerializeComponent);

		// Add Component Overrides to a new Entity
		rapidjson::Value entityObj;
		entityObj.SetObject();
		entityObj.AddMember("Overrides", compOverridesObj, json.GetAllocator());

		// Give the entity a name
		local_persistent char eid_str[128];
		auto len = sprintf_s(eid_str, "Entity%llu", entity.Id());
		rapidjson::Value entity_name;
		entity_name.SetString(eid_str, len, json.GetAllocator());

		// If the doc already has an Entity by the same name, swap it out. Otherwise, add it.
		auto doc_itr = json.FindMember(entity_name);
		if (doc_itr != json.MemberEnd())
		{
			doc_itr->value.Swap(entityObj);
		}
		else
		{
			json.AddMember(entity_name, entityObj, json.GetAllocator());
		}
	}

}