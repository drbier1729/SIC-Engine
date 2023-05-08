/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

// included in Entities.h

#include <cassert>
#include <functional>

namespace sic {

// Helper for creating an entity type bitset based on a typelist of components
	namespace detail {

		template<size_t N, std::unsigned_integral ... Args>
		constexpr std::bitset<N> MakeBitMask(void)
		{
			constexpr std::bitset<N> one_bit{ 1 };
			return ((one_bit << Args) | ...);
		}
	}

	template<ValidComponent...Cs>
	constexpr std::bitset<NUM_COMPONENTS> GetEntityType(void)
	{
		return detail::MakeBitMask<NUM_COMPONENTS, CompId<Cs>()...>();
	}

}

namespace sic {

	template<ValidComponent C>
	C& Entity::Get(void) const
	{
		assert(IsValid());

		auto c_ptr = m_ecs_ref->GetComponent<C>(id);
		assert(c_ptr != nullptr && "Entity does not have this component.");

		return *c_ptr;
	}

	template<ValidComponent C>
	bool Entity::Has(void) const
	{
		return m_ecs_ref->HasComponent(C::id, id);
	}

	template<ValidComponent C>
	C& Entity::Add(void) const
	{
		assert(IsValid());

		auto c_ptr = m_ecs_ref->AddComponent<C>(id); // will not be null if Entity is valid
		assert(c_ptr != nullptr && "This should never happen: Entity::IsValid but AddComponent failed.");
		
		return *m_ecs_ref->AddComponent<C>(id);
	}

	template<ValidComponent C>
	void Entity::Remove(void) const
	{
		assert(IsValid());
		m_ecs_ref->RemoveComponent<C>(id);
	}

	//template<ValidComponent C1, ValidComponent... Cs>
	//std::tuple<C1&, Cs&...>
	//Entity::Get(void) const
	//{

	//}

	template<ValidComponent... Cs>
	bool Entity::HasAll(void) const
	{
		local_persistent constexpr auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref->m_entityTypes[id] };
		return ((archetype & entity_type) == archetype);
	}

	template<ValidComponent... Cs>
	bool Entity::HasExactly(void) const
	{
		local_persistent constexpr auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref->m_entityTypes[id] };
		return (archetype == entity_type);
	}

	template<ValidComponent... Cs>
	bool Entity::HasAny(void) const
	{
		local_persistent constexpr auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref->m_entityTypes[id] };
		return ((archetype & entity_type) != 0);
	}

	template<ValidComponent... Cs>
	bool Entity::HasNone(void) const
	{
		local_persistent constexpr auto archetype{ GetEntityType<Cs...>() };
		const auto entity_type{ m_ecs_ref->m_entityTypes[id] };
		return ((archetype & entity_type) == 0);
	}


	template<typename F>
	void Entity::ForEachComponent(F fn) const
	{
		assert(IsValid());	
		auto eid = m_ecs_ref->GetIdFromEntity(id);
															    // Have to specify the first arg explicitly so C can be deduced
		auto CallOnComponent = [this, eid, fn]<ValidComponent C>(plf::colony<C, std::pmr::polymorphic_allocator<C>>& component_data, ComponentID cid) -> void
		{
			if (m_ecs_ref->m_entityTypes[eid].test(cid))
			{
				C* comp = reinterpret_cast<C*>(m_ecs_ref->m_sparse[eid][cid]);
				assert(comp != nullptr && "Something is wrong: entity type does not match component data.");
				fn(*comp);
			}
		};
		tuple_for_each_idx(m_ecs_ref->m_dense, CallOnComponent);
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

	EntityID EntityAdmin::GetOwnerID(ValidComponent auto const& component)
	{
		return component.owner;
	}

	template<ValidComponent C>
	plf::colony<C, std::pmr::polymorphic_allocator<C>>& EntityAdmin::GetComponentArray(void)
	{
		return std::get<plf::colony<C, std::pmr::polymorphic_allocator<C>>>(m_dense);
	}


	template<ValidComponent C, typename F>
	requires std::invocable<F&, C&, f32> || std::invocable<F&, C&>
	void EntityAdmin::ForEach(F fn, f32 dt)
	{
		auto& component_data = GetComponentArray<C>();
		if constexpr (!std::invocable<F&, C&, f32>)
		{
			for (auto it = component_data.begin(); it != component_data.end(); ++it)
			{
				fn(*it);
			}
		}
		else
		{
			for (auto it = component_data.begin(); it != component_data.end(); ++it)
			{
				fn(*it, dt);
			}
		}
	}


	template<typename F>
	requires std::invocable<F&, Entity const&>
		void EntityAdmin::ForEachEntity(F fn)
	{
		size_t counter{ 0 };
		std::for_each(std::begin(m_idGenerations), std::end(m_idGenerations),
			[=, this, &counter](i64 const& gen)
			{
				assert(counter <= m_idCounter);
				if (gen > 0)
				{
					EntityID eid{ GenerateEntityFromIDandGeneration(counter, gen) };
					fn(GetEntityFromID(eid));
				}
				++counter;
			});
	}


	// Private
	template<ValidComponent C>
	C* EntityAdmin::GetComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity index is valid
		assert(id < m_idCounter);

		// If the entity is no longer active, return nullptr
		if (gen != m_idGenerations[id] || gen < 0)
		{
			return nullptr;
		}

		return reinterpret_cast<C*>(m_sparse[id][C::id]); // may be nullptr
	}

	template<ValidComponent C>
	C* EntityAdmin::AddComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity is valid
		assert(id < m_idCounter);

		// If our entity is no longer active, return nullptr
		if (gen != m_idGenerations[id] || gen < 0)
		{
			return nullptr;
		}

		// If the entity has this component already, just return it
		if (m_entityTypes[id].test(C::id))
		{
			C* comp{ reinterpret_cast<C*>(m_sparse[id][C::id]) };
			assert(comp != nullptr && "Something went wrong: m_entityTypes and m_sparse disagree.");
			return comp;
		}

		// Change entity type
		m_entityTypes[id].set(C::id);

		// Get dense array
		auto& component_data{ GetComponentArray<C>() };

		// Construct new component and set its owner
		auto it = component_data.emplace();
		C* c_ptr = reinterpret_cast<C*>(&(*it));
		c_ptr->owner = eid;

		// Set ptr in sparse array
		m_sparse[id][C::id] = c_ptr;

		return c_ptr;
	}


	template<ValidComponent C>
	void EntityAdmin::RemoveComponent(EntityID eid)
	{
		i64 gen{ GetGenerationFromEntity(eid) };
		u64 id{ GetIdFromEntity(eid) };

		// Assert our entity is valid
		assert(id < m_idCounter);

		// If entity is no longer active, or does not have the component, just return
		if (gen != m_idGenerations[id] || gen < 0 || !m_entityTypes[id].test(C::id))
		{
			return;
		}

		// -- Remove component from type info --
		m_entityTypes[id].reset(C::id);

		// -- Remove components from the SparseSet --
		auto& component_data{ GetComponentArray<C>() };
		
		C* current{ reinterpret_cast<C*>(m_sparse[id][C::id]) };
		assert(current != nullptr && "Something went wrong: m_entityTypes and m_sparse disagree.");
		
		auto it = component_data.get_iterator(current);
		component_data.erase(it);

		m_sparse[id][C::id] = nullptr;
	}
}