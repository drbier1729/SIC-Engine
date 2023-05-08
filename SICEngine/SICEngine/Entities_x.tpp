/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#pragma once

#include <cassert>
#include <functional>

namespace sic {
	// utilities
	
	template<ValidComponent...Cs>
	std::bitset<NUM_COMPONENTS> GetEntityType(void)
	{
		return detail::MakeBitMask<NUM_COMPONENTS, CompId<Cs>()...>();
	}
}

namespace sic {

	template<ValidComponent C>
	C&
	Entity::Get(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		return m_ecs_ref.GetComponent<C>(id);
	}

	template<ValidComponent C>
	bool
	Entity::Has(void) const
	{
		return m_ecs_ref.HasComponent(C::id, id);
	}

	template<ValidComponent C>
	C&
	Entity::Add(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		return m_ecs_ref.AddComponent<C>(id);
	}

	template<ValidComponent C>
	void
	Entity::Remove(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		return m_ecs_ref.RemoveComponent<C>(id);
	}

	//template<ValidComponent C1, ValidComponent... Cs>
	//std::tuple<C1&, Cs&...>
	//Entity::Get(void) const
	//{

	//}

	template<ValidComponent... Cs>
	bool
	Entity::HasAll(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		const auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref.m_entityTypes[id] };
		return ((archetype & entity_type) == archetype);
	}

	template<ValidComponent... Cs>
	bool
	Entity::HasExactly(void) const 
	{
		assert(m_ecs_ref.IsValidEntity(id));
		const auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref.m_entityTypes[id] };
		return (archetype == entity_type);
	}

	template<ValidComponent... Cs>
	bool
	Entity::HasAny(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		const auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref.m_entityTypes[id] };
		return ((archetype & entity_type) != 0);
	}

	template<ValidComponent... Cs>
	bool
	Entity::HasNone(void) const
	{
		assert(m_ecs_ref.IsValidEntity(id));
		const auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref.m_entityTypes[id] };
		return ((archetype & entity_type) == 0);
	}


	template<typename F>
	void Entity::ForEachComponent(F fn) const
	{
		assert(IsValid());
		auto CallOnComponent = [this, fn](auto& component_data, ComponentID cid) -> void
		{
			u64 eid = m_ecs_ref.GetIdFromEntity(id);
			if (m_ecs_ref.m_entityTypes[eid].test(cid))
			{
				u64 key = m_ecs_ref.m_sparse[eid][cid];
				fn(component_data[key]);
			}
		};

		tuple_for_each_idx(m_ecs_ref.m_dense, CallOnComponent);
	}
	//template<ValidComponent C1, ValidComponent... Cs>
	//std::tuple<C1&, Cs&...>
	//Entity::Add(void) const 
	//{

	//}

	//template<ValidComponent C1, ValidComponent... Cs>
	//bool
	//Entity::Remove(void) const 
	//{

	//}
}

namespace sic {

	Entity EntityAdmin::GetOwner(ValidComponent auto const& component)
	{
		return Entity{ *this, component.owner };
	}

	template<ValidComponent C>
	std::pmr::vector<C>& EntityAdmin::GetComponentArray(void)
	{
		return std::get<std::pmr::vector<C>>(m_dense);
	}

	template<ValidComponent C, typename F>
	requires std::invocable<F&, C&>
	void EntityAdmin::ForEach(F fn)
	{
		auto& component_data{ GetComponentArray<C>() };
		std::for_each(component_data.begin(), component_data.end(), fn);
	}


	template<typename F>
	requires std::invocable<F&, Entity const&>
	void EntityAdmin::ForEachEntity(F fn)
	{
		size_t counter{ 0 };
		std::for_each(std::begin(m_idGenerations), std::end(m_idGenerations),
			[=, this, &counter] (i64 const& gen) 
			{
				assert(counter <= m_idCounter);
				EntityID eid{ GenerateEntityFromIDandGeneration(counter++, gen) };
				if (IsValidEntity(eid))
				{
					fn(GetEntityFromID(eid));
				}
			});
	}


	// Private
	template<ValidComponent C>
	C& EntityAdmin::GetComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity is valid and still active
		assert(id < m_idCounter);
		assert(gen == m_idGenerations[id] && gen > 0);

		// Make sure entity has this component
		assert(m_entityTypes[id].test(C::id));

		auto& component_data{ GetComponentArray<C>() };
		u64 key{ m_sparse[id][C::id] };

		// Sanity check
		assert(key < component_data.size());
		
		return component_data[key];
	}


	// TODO IMPORTANT (dylan) : crash here in Release build
	template<ValidComponent C>
	C& EntityAdmin::AddComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity is valid and still active
		assert(id < m_idCounter);
		assert(gen == m_idGenerations[id] && gen >=0);

		// Make sure entity does not have this component already
		assert(!m_entityTypes[id].test(C::id));

		// Change entity type
		m_entityTypes[id].set(C::id);

		// Get dense array
		auto& component_data{ GetComponentArray<C>() };

		// Set index in sparse array
		m_sparse[id][C::id] = component_data.size();

		// Construct new component at back of dense array and set its owner
		component_data.emplace_back();
		component_data.back().owner = eid;

		return component_data.back();
	}


	template<ValidComponent C>
	void EntityAdmin::RemoveComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity is valid and still active
		assert(id < m_idCounter);
		assert(gen == m_idGenerations[id] && gen > 0);

		if (m_entityTypes[id].test(C::id))
		{
			// -- Remove component from type info --
			m_entityTypes[id].reset(C::id);

			// -- Remove components from the SparseSet --
			u64& current_key{ m_sparse[id][C::id] };

			auto& component_data{ GetComponentArray<C>() };
			auto& current{ component_data[current_key] };
			auto& last{ component_data.back() };

			// Copy component data from the component at the end of the vector
			current = last;
			current.owner = last.owner; // owner needs to be copied separately

			// Update the sparse table so the key is correct for the component
			// that just moved
			u64& last_key{ m_sparse[GetIdFromEntity(last.owner)][C::id] };
			last_key = current_key;

			// Invalidate the old key
			current_key = MAX_ACTIVE_ENTITIES;

			// Shrink the dense array
			component_data.pop_back();
		}
	}
}