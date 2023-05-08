/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#pragma once

#include <queue>
#include <bitset>

#include "SICTypes.h"
#include "Components.h"


// Entity Handle
namespace sic {

	// Note: this is only a Handle to an Entity! If it is destroyed, the EntityAdmin will
	// not be affected in any way!
	struct Entity
	{
	public:
		friend class EntityAdmin;

	public:
		template<ValidComponent C>
		C&   Get(void) const;

		template<ValidComponent C>
		bool Has(void) const;

		template<ValidComponent C>
		C&   Add(void) const;

		template<ValidComponent C>
		void Remove(void) const;

		EntityID Id(void) const;
		bool     IsValid(void) const;
		u64		 Index(void) const;

		// Get/Check multiple components
		template<ValidComponent... Cs>
		bool
		HasAll(void) const;

		template<ValidComponent... Cs>
		bool
		HasExactly(void) const;
		
		template<ValidComponent... Cs>
		bool
		HasAny(void) const;

		template<ValidComponent... Cs>
		bool
		HasNone(void) const;

		// TODO : Requires F to be invocable on any ValidComponent
		template<typename F>
		void ForEachComponent(F fn) const;



		//template<ValidComponent C1, ValidComponent... Cs>
		//std::tuple<C1&, Cs&...> 
		//Get(void) const;

		//// Add/Remove multiple components

		//template<ValidComponent C1, ValidComponent... Cs>
		//std::tuple<C1&, Cs...> 
		//Add(void) const;

		//template<ValidComponent C1, ValidComponent... Cs>
		//bool
		//Remove(void) const;


	public:
		Entity(EntityAdmin& ecs);
		Entity(Entity const& src) = default;
		Entity(Entity&& src) = default;
		Entity& operator=(Entity const& rhs);
		Entity& operator=(Entity&& rhs) noexcept;

		~Entity() = default;

	private:
		// Can only create valid Entities using EntityAdmin::MakeEntity
		Entity(EntityAdmin& ecs, EntityID id);

	private:
		EntityAdmin& m_ecs_ref; // TODO (dylan) : this should be a ptr or reference wrapper?
		EntityID id;
	};
}


// Entity Admin
namespace sic {

	class EntityAdmin
	{
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;
		using entity_type = std::bitset<NUM_COMPONENTS>;

		friend struct Entity;
		friend class Factory;
		friend class ImGuiWindow;

	public:
		Entity MakeEntity(void);
		void   DestroyEntity(Entity const& entity);
		
		Entity GetEntityFromID(EntityID const& entity_id);
		Entity GetOwner(ValidComponent auto const& component);
		bool HasComponent(ComponentID cid, EntityID eid);
		bool IsValidEntity(EntityID eid);

		template<typename F>
		requires std::invocable<F&, Entity const&>
		void ForEachEntity(F fn);

		template<ValidComponent C, typename F>
		requires std::invocable<F&, C&>
		void ForEach(F fn);

		// WARNING: Are you sure wanna use this and not ForEach?
		template<ValidComponent C>
		std::pmr::vector<C>& GetComponentArray(void);

		allocator_type GetAllocator(void) const noexcept { return m_allocator; }

	public:
		EntityAdmin();
		explicit EntityAdmin(allocator_type alloc);
		~EntityAdmin();

		void Reset(void);
	
	public:
		EntityAdmin(EntityAdmin const& src, allocator_type alloc = {}) = delete;
		EntityAdmin(EntityAdmin&& src) = delete;
		EntityAdmin(EntityAdmin&& src, allocator_type alloc) = delete;
		EntityAdmin& operator=(EntityAdmin const& rhs) = delete;
		EntityAdmin& operator=(EntityAdmin&& rhs) = delete;

	private:	
		template<ValidComponent C>
		C& GetComponent(EntityID eid);

		template<ValidComponent C>
		C& AddComponent(EntityID eid);

		void AddComponent(EntityID eid, ComponentID cid);

		template<ValidComponent C>
		void RemoveComponent(EntityID eid);

		void RemoveComponent(EntityID eid, ComponentID cid);

	private:
		// Helpers		
		EntityID GenerateEntityFromIDandGeneration(u64 id, i64 generation) const;
		i64 GetGenerationFromEntity(EntityID entity) const;
		u64 GetIdFromEntity(EntityID entity) const;

		void RemoveAllComponents(u64 entity_index);

	private:
		allocator_type m_allocator;

		// Entity IDs:
		u64 m_numActiveEntities;
		u64 m_idCounter;
		std::priority_queue<u64, std::pmr::vector<u64>, std::greater<>> m_reusableIds;
		std::pmr::vector<i64> m_idGenerations;
			// How it works:
			// When density of active entities to idCounter is high, Entity ids are created
			// by simply incrementing m_idCounter. When the density is low, ids of destroyed
			// entities are reused by popping from m_reusableIds. m_idGenerations is used
			// to track how many times an id has been reused. 
			// When MakeEntity() is called, it gives each entity a 64bit id where the lower 32bits are 
			// an index into the m_compSparseTable, and the upper 32bits are the generation
			// count of that index (with sign bit indicating if the entity is active or not).
		

		// Entity Types:
		std::pmr::vector<entity_type> m_entityTypes;
			// How it works:
			// m_entityTypes[Entity id] = bitset of components that that entity "has"

		// Component Data (SparseSet):
		using KeyArray = std::pmr::vector<u64>;
		using SparseTable = std::pmr::vector<KeyArray>; // one KeyArray per active entity. indexed by entity id.
		using DenseTable = SOA<Components>::type; // tuple of vectors. one vector for each component type.

		SparseTable m_sparse;
		DenseTable m_dense;
		// How it works:
			// m_sparse[Entity id][Component id] = key
			// m_dense[Component id][key] = instance of a component for a specific entity
	};
}

#include "Entities_x.tpp"