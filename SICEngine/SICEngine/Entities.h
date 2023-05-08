/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <queue>
#include <bitset>
#include <cassert>

#include "SICTypes.h"
#include "Components.h"


// Entity Handle
namespace sic {

	class EntityAdmin;
	struct Entity;

	// Note: this is only a Handle to an Entity! If it is destroyed, the EntityAdmin will
	// not be affected in any way!
	struct Entity
	{
	public:
		friend class EntityAdmin;

	public:
		inline EntityID Id(void) const { return id; }
		bool IsValid(void) const;

		template<ValidComponent C>
		C& Get(void) const;

		template<ValidComponent C>
		bool Has(void) const;

		template<ValidComponent C>
		C& Add(void) const;

		template<ValidComponent C>
		void Remove(void) const;

		template<ValidComponent... Cs>
		bool HasAll(void) const;

		template<ValidComponent... Cs>
		bool HasExactly(void) const;

		template<ValidComponent... Cs>
		bool HasAny(void) const;

		template<ValidComponent... Cs>
		bool HasNone(void) const;

		// TODO : Requires F to be invocable on any ValidComponent
		template<typename F>
		void ForEachComponent(F fn) const;

		//template<ValidComponent C1, ValidComponent... Cs>
		//std::tuple<C1&, Cs&...> Get(void) const;

		//template<ValidComponent C1, ValidComponent... Cs>
		//std::tuple<C1&, Cs...> Add(void) const;

		//template<ValidComponent C1, ValidComponent... Cs>
		//bool Remove(void) const;


	public:
		Entity(EntityAdmin& ecs) : m_ecs_ref{ &ecs }, id{ NullEntityId } { }
		Entity(Entity const& src) = default;
		Entity(Entity&& src) = default;
		Entity& operator=(Entity const& rhs);
		Entity& operator=(Entity&& rhs) noexcept;

		~Entity() = default;

	private:
		// Can only create valid Entities using EntityAdmin::MakeEntity
		Entity(EntityAdmin& ecs, EntityID id_) : m_ecs_ref{ &ecs }, id{ id_ } { }

	private:
		EntityAdmin* m_ecs_ref = nullptr;
		EntityID id = NullEntityId;
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
		friend class PhysicsManager;
		friend class ImGuiWindow;

	public:
		Entity MakeEntity(void);
		void   DestroyEntity(Entity const& entity);
		
		Entity GetEntityFromID(EntityID const& entity_id);
		Entity GetOwner(ValidComponent auto const& component);
		EntityID GetOwnerID(ValidComponent auto const& component);

		bool IsValidEntity(EntityID eid);

		template<typename F>
		requires std::invocable<F&, Entity const&>
		void ForEachEntity(F fn);

		template<ValidComponent C, typename F>
		requires std::invocable<F&, C&, f32> || std::invocable<F&, C&>
		void ForEach(F fn, f32 dt = 0.0f);

		// WARNING: Are you sure wanna use this and not ForEach?
		template<ValidComponent C>
		plf::colony<C, std::pmr::polymorphic_allocator<C>>& GetComponentArray(void);

		inline allocator_type GetAllocator(void) const { return m_allocator; }

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

		// TODO : These should take an entity_index as arg, not entity_id. Index and generation
		// should already be extracted before calling one of these methods
		bool HasComponent(ComponentID cid, EntityID eid);

		template<ValidComponent C>
		C* GetComponent(EntityID eid);

		template<ValidComponent C>
		C* AddComponent(EntityID eid);

		void* AddComponent(EntityID eid, ComponentID cid);

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
		using KeyArray = std::pmr::vector<void*>;
		using SparseTable = std::pmr::vector<KeyArray>; // one KeyArray per active entity. indexed by entity id.
		using DenseTable = Hive<Components>::type;

		SparseTable m_sparse;
		DenseTable m_dense;
		// How it works:
			// m_sparse[Entity id][Component id] = ptr to Component instance stored in m_dense
	};
}

#include "Entities.tpp"