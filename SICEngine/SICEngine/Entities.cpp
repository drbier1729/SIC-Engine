/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#include "Entities.h"

#include <iostream>
#include <algorithm>


// Entity non-template methods
namespace sic {

	Entity& Entity::operator=(Entity const& rhs)
	{
		assert(m_ecs_ref == rhs.m_ecs_ref && "Entities belong to different EntityAdmin objects.");
		id = rhs.id;
		return *this;
	}

	Entity& Entity::operator=(Entity&& rhs) noexcept
	{
		assert(m_ecs_ref == rhs.m_ecs_ref && "Entities belong to different EntityAdmin objects.");
		id = rhs.id;
		return *this;
	}

	bool Entity::IsValid(void) const { assert(m_ecs_ref != nullptr); return m_ecs_ref->IsValidEntity(id); }
}


// EntityAdmin non-template methods
namespace sic {

	static constexpr size_t ENTITY_RESERVE{ 64 };

	EntityAdmin::EntityAdmin() : EntityAdmin(allocator_type{}) {}

	EntityAdmin::EntityAdmin(allocator_type alloc)
		: m_allocator{ alloc },
		m_numActiveEntities{ 0 },
		m_idCounter{ 0 },
		m_reusableIds{ alloc },
		m_idGenerations{ alloc },

		m_entityTypes{ alloc },

		m_sparse{ alloc },
		m_dense{ std::allocator_arg, alloc }
	{
		m_idGenerations.reserve(ENTITY_RESERVE);

		m_entityTypes.reserve(ENTITY_RESERVE);

		m_sparse.reserve(ENTITY_RESERVE);

	}

	EntityAdmin::~EntityAdmin()
	{
		Reset();
	}

	Entity EntityAdmin::MakeEntity(void)
	{
		assert(m_idCounter < MAX_ACTIVE_ENTITIES - 1);

		EntityID entity_id{ NullEntityId };
		i64 gen{ 1 };
		u64 id{ 0 };

		// Low density
		if (m_numActiveEntities < m_idCounter / 2 && !m_reusableIds.empty())
		{
			// Grab the first id available for reuse
			id = m_reusableIds.top();
			m_reusableIds.pop();

			// Get the generation count of this id
			m_idGenerations[id] = -1 * m_idGenerations[id] + 1;
			gen = m_idGenerations[id];
			assert(gen < INT32_MAX && gen > 0);

			// Create the new entity_id
			entity_id = GenerateEntityFromIDandGeneration(id, gen);
		}

		// High density
		else
		{
			id = m_idCounter;
			++m_idCounter;

			// Create the new entity_id. Generation = 1.
			entity_id = GenerateEntityFromIDandGeneration(id, gen);;

			// Set the entity_id's generation
			m_idGenerations.emplace_back(gen);

			// Give the entity null type
			m_entityTypes.emplace_back(0); // 0 bitset

			m_sparse.emplace_back(KeyArray(NUM_COMPONENTS, nullptr, m_allocator)); // create sparse vector for entity
		}

		// Increment the active entity_id count
		++m_numActiveEntities;

		// Debug Log
		//std::cout << "EntityID: [ " << id << " ]  Generation: [ " << gen << " ]\n";

		return Entity{ *this, entity_id };
	}

	Entity EntityAdmin::GetEntityFromID(EntityID const& entity_id)
	{
		// TODO : do we want an assertion or a client-side if( GetEntityFromID( id ).IsValid() ) {...} ?
		assert(IsValidEntity(entity_id));

		return Entity{ *this, entity_id };
	}

	void EntityAdmin::DestroyEntity(Entity const& entity)
	{
		EntityID entity_id{ entity.id };
		i64 gen{ GetGenerationFromEntity(entity_id) };
		u64 id{ GetIdFromEntity(entity_id) };
		
		// Assert our entity is valid
		assert(id < m_idCounter);
		
		// If entity is inactive, do nothing
		if (gen != m_idGenerations[id] || gen < 0) { return; }

		// Remove components
		RemoveAllComponents(id);

		// Reset entity's type
		m_entityTypes[id].reset();

		// Prepare id for reuse
		m_idGenerations[id] *= -1;
		m_reusableIds.push(id);

		// Decrement entity count
		--m_numActiveEntities;
	}


	// Private
	bool  EntityAdmin::HasComponent(ComponentID cid, EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity id is valid
		assert(id < m_idCounter);

		// If the generation is outdated, the entity was destroyed, so return false
		if (gen != m_idGenerations[id] || gen < 0) { return false; }

		// Otherwise, test to see if the entity has the desired component
		else { return m_entityTypes[id].test(cid); }
	}

	void* EntityAdmin::AddComponent(EntityID eid, ComponentID cid)
	{
		assert(cid < NUM_COMPONENTS);

		ComponentType compType{ static_cast<ComponentType>(cid) };

		void* comp = nullptr;

		switch (compType)
		{
#define REGISTER_COMPONENT(name) \
			case ComponentType::##name:\
			{\
				comp = AddComponent<name>(eid);\
				break;\
			}
#include "Components.def"
#undef REGISTER_COMPONENT
		default:
		{
			break;
		}
		}

		return comp;
	}

	void EntityAdmin::RemoveComponent(EntityID eid, ComponentID cid)
	{
		assert(cid < NUM_COMPONENTS);

		ComponentType compType{ static_cast<ComponentType>(cid) };

		switch (compType)
		{
#define REGISTER_COMPONENT(name) \
			case ComponentType::##name:\
			{\
				RemoveComponent<name>(eid);\
				break;\
			}
#include "Components.def"
#undef REGISTER_COMPONENT
		default:
		{
			break;
		}
		}
	}

	void EntityAdmin::Reset(void)
	{
		m_entityTypes.clear();

		m_sparse.clear();
		tuple_for_each_idx(m_dense, [](auto& component_data, ComponentID id) -> void
		{
			component_data.clear();
		});

		m_numActiveEntities = 0;
		m_idCounter = 0;
		m_idGenerations.clear();
		std::priority_queue< u64, std::pmr::vector<u64>, std::greater<>> new_queue{};
		m_reusableIds.swap(new_queue);
	}

	bool EntityAdmin::IsValidEntity(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		if (id < m_idCounter)
		{
			return (gen == m_idGenerations[id] && gen > 0);
		}
		return false;
	}


	EntityID EntityAdmin::GenerateEntityFromIDandGeneration(u64 id, i64 generation) const
	{
		return ((generation << 0x20) & ENTITY_GEN_MASK) | (id & ENTITY_ID_MASK);
	}

	u64 EntityAdmin::GetIdFromEntity(EntityID eid) const
	{
		return (eid & ENTITY_ID_MASK);
	}

	i64 EntityAdmin::GetGenerationFromEntity(EntityID eid) const
	{
		return ((eid & ENTITY_GEN_MASK) >> 0x20);
	}

	void EntityAdmin::RemoveAllComponents(u64 id)
	{
		auto RemoveFromSparseSet = [=, this]<ValidComponent C>(plf::colony<C, std::pmr::polymorphic_allocator<C>>& component_data, ComponentID cid) -> void {

			if (m_entityTypes[id].test(cid))
			{
				C* current{ reinterpret_cast<C*>(m_sparse[id][cid]) };
				assert(current != nullptr && "Something went wrong: m_entityTypes and m_sparse disagree.");
				
				auto it = component_data.get_iterator(current);
				component_data.erase(it);
				m_sparse[id][cid] = nullptr;
			}
		};

		tuple_for_each_idx(m_dense, RemoveFromSparseSet);
	}
}