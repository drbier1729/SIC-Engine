/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "Entities_x.h"

#include <cassert>
#include <iostream>
#include <algorithm>


// Entity non-template methods
namespace sic {

	Entity::Entity(EntityAdmin& ecs) 
		: m_ecs_ref{ ecs }, id{ NullEntityId } 
	{ }

	// Private
	Entity::Entity(EntityAdmin& ecs, EntityID id_)
		: m_ecs_ref{ ecs }, id{ id_ }
	{ }

	Entity& Entity::operator=(Entity const& rhs)
	{
		assert(&m_ecs_ref == &rhs.m_ecs_ref && "Entities belong to different EntityAdmin objects.");
		id = rhs.id;
		return *this;
	}

	Entity& Entity::operator=(Entity&& rhs) noexcept
	{
		assert(&m_ecs_ref == &rhs.m_ecs_ref && "Entities belong to different EntityAdmin objects.");
		id = rhs.id;
		return *this;
	}

	EntityID Entity::Id(void) const { return id; }
	bool Entity::IsValid(void) const { return m_ecs_ref.IsValidEntity(id); }
	u64  Entity::Index(void) const { return m_ecs_ref.GetIdFromEntity(id); }
}


// EntityAdmin non-template methods
namespace sic {

	static constexpr size_t ENTITY_RESERVE{ 64 };

	EntityAdmin::EntityAdmin() : EntityAdmin(allocator_type{}) {}

	EntityAdmin::EntityAdmin(allocator_type alloc)
		: m_allocator{ alloc },
		/* m_world{world},*/
		m_numActiveEntities{ 0 },
		m_idCounter{0},
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
		if (m_numActiveEntities < m_idCounter/2 && !m_reusableIds.empty())
		{
			// Grab the first id available for reuse
			id = m_reusableIds.top();
			m_reusableIds.pop();

			// Get the generation count of this id
			m_idGenerations[id] = -1*m_idGenerations[id] + 1;
			gen = m_idGenerations[id];
			assert(gen < INT32_MAX && gen > 0);

			// Create the new entity_id
			entity_id = GenerateEntityFromIDandGeneration(id, gen);

			// Type and TypeArray were reset when the last entity using this id was destroyed
			
			// Create new, empty key array at id
			std::pmr::vector<EntityID> new_keys(NUM_COMPONENTS, MAX_ACTIVE_ENTITIES, m_allocator);
			m_sparse[id].swap(new_keys);
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
		
			m_sparse.emplace_back(std::pmr::vector<u64>(NUM_COMPONENTS, MAX_ACTIVE_ENTITIES, m_allocator)); // create sparse vector for entity
		}

		// Increment the active entity_id count
		++m_numActiveEntities;

		// Debug Log
		//std::cout << "EntityID: [ " << id << " ]  Generation: [ " << gen << " ]\n";

		return Entity{ *this, entity_id };
	}

	Entity EntityAdmin::GetEntityFromID(EntityID const& entity_id)
	{
		assert(IsValidEntity(entity_id));

		return Entity{ *this, entity_id };
	}

	void EntityAdmin::DestroyEntity(Entity const& entity)
	{
		EntityID entity_id{ entity.id };
		i64 gen{ GetGenerationFromEntity(entity_id) };
		u64 id{ GetIdFromEntity(entity_id) };

		// Assert our entity is valid and still active
		assert(id < m_idCounter);
		assert(gen == m_idGenerations[id] && gen > 0);

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

		// TODO (dylan) : should this be an assert?
		// If the generation is outdated, the entity was destroyed, so return false
		if (gen != m_idGenerations[id] || gen < 0) { return false; }

		// Otherwise, test to see if the entity has the desired component
		else { return m_entityTypes[id].test(cid); }
	}

	void EntityAdmin::AddComponent(EntityID eid, ComponentID cid)
	{
		assert(cid < NUM_COMPONENTS);
		assert(IsValidEntity(eid));

		ComponentType compType{ static_cast<ComponentType>(cid) };

		switch (compType)
		{
#define REGISTER_COMPONENT(name) \
			case ComponentType::##name:\
			{\
				AddComponent<name>(eid);\
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
	
	void EntityAdmin::RemoveComponent(EntityID eid, ComponentID cid)
	{
		assert(cid < NUM_COMPONENTS);
		assert(IsValidEntity(eid));

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
		auto RemoveFromSparseSet = [=, this](auto& component_data /*vector*/, ComponentID cid) -> void {
			
			if (m_entityTypes[id].test(cid))
			{
				u64& current_key{ m_sparse[id][cid] };
				auto& current{ component_data[current_key] };
				auto& last{ component_data.back() };

				// Copy component data from the component at the end of the vector
				current = last;
				current.owner = last.owner; // owner needs to be copied separately

				// Update the sparse table so the key is correct for the component
				// that just moved
				u64& last_key{ m_sparse[GetIdFromEntity(last.owner)][cid] };
				last_key = current_key;

				// Invalidate the old key
				current_key = MAX_ACTIVE_ENTITIES;

				// Shrink the dense array
				component_data.pop_back();
			}
		};

		tuple_for_each_idx(m_dense, RemoveFromSparseSet);
	}
}