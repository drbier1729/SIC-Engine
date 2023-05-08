/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

//#pragma once
//
//#include <vector>
//#include <concepts>
//
//#include "ComponentBase.h"
//
//
//namespace sic {
//	
//	// TODO (dylan) : EntityType doesn't work for even number of components >2
//	
//	template<ValidComponent C>
//	constexpr size_t EntityType(void)
//	{
//		return (1ull << C::id);
//	}
//
//	template<ValidComponent First, ValidComponent Second>
//	constexpr size_t EntityType(void)
//	{
//		return ((1ull << First::id) | (1ull << Second::id)) ;
//	}
//
//	template<ValidComponent First, ValidComponent Second, ValidComponent ... Rest>
//	constexpr size_t EntityType(void)
//	{
//		return ( ((1ull << First::id) | (1ull << Second::id)) | EntityType<Rest...>() );
//	}
//}
//
//// Archetype Data Structures
//namespace sic
//{
//	class ArchetypeBase
//	{
//	public:
//		using allocator_type = std::pmr::polymorphic_allocator<>;
//
//		friend class ArchetypeAdmin;
//
//	protected:
//		ArchetypeBase(size_t entityType_) : ArchetypeBase(entityType_, allocator_type{}) {}
//		explicit ArchetypeBase(size_t entityType_, allocator_type alloc)
//			: entityType(entityType_),
//			entityIDs(alloc)
//		{}
//
//		virtual ~ArchetypeBase() = default;
//
//	protected:
//		size_t entityType;
//		std::pmr::vector<EntityID> entityIDs;
//
//	private:
//		ArchetypeBase(ArchetypeBase const& src, allocator_type alloc = {}) = delete;
//		ArchetypeBase(ArchetypeBase && src) = delete;
//		ArchetypeBase(ArchetypeBase && src, allocator_type alloc) = delete;
//		ArchetypeBase& operator=(ArchetypeBase const& rhs) = delete;
//		ArchetypeBase& operator=(ArchetypeBase && rhs) = delete;
//	};
//
//
//	template<ValidComponent ... Cs>
//	class Archetype final : public ArchetypeBase
//	{
//	public:
//		using allocator_type = std::pmr::polymorphic_allocator<>;
//		using Table = SOA<TypeList<Cs...>>::type;
//
//		Archetype() : Archetype(allocator_type{}) {}
//		explicit Archetype(allocator_type alloc)
//			: ArchetypeBase{ EntityType<Cs...>(), alloc },
//			componentData{ std::allocator_arg, alloc }
//		{}
//
//		~Archetype() = default;
//
//
//		template<ValidComponent C, ValidComponent ... Rest>
//		std::tuple<std::pmr::vector<C>&, std::pmr::vector<Rest>&...>
//		Get(void) 
//		{ 
//			return std::tuple(std::ref(std::get<std::pmr::vector<C>>(componentData)), 
//				std::ref(std::get<std::pmr::vector<Rest>>(componentData))...); 
//		}
//
//		template<ValidComponent ... Cs>
//		void PushBack(EntityID id)
//		{
//			auto emplace_back = [](std::pmr::vector<Cs>&... vecs) -> void { vecs.emplace_back(); };
//			std::apply(emplace_back, componentData);
//			entityIDs.push_back(id);
//		}
//
//	private:
//		Table componentData;
//	};
//
//
//	class ArchetypeAdmin
//	{
//	public:
//		using allocator_type = std::pmr::polymorphic_allocator<>;
//
//		ArchetypeAdmin() : ArchetypeAdmin(allocator_type{}) {}
//		explicit ArchetypeAdmin(allocator_type alloc) : m_archetypes(alloc), m_allocator(alloc) {}
//		~ArchetypeAdmin() = default;
//
//
//		template<typename F, ValidComponent ... Cs>
//		requires std::invocable<F&, Cs&...>
//		void Each(F fn);
//
//		template<ValidComponent...Cs>
//		Archetype<Cs...>& GetOrAddArchetype(void);
//
//		template<ValidComponent...Cs>
//		void PushEntity(Entity const& e);
//
//
//	private:		
//
//		template<ValidComponent ... Cs>
//		Archetype<Cs...>& AddArchetype(size_t compBitset);
//
//
//	private:
//		std::pmr::vector<std::pair<size_t, ArchetypeBase*>> m_archetypes;
//		allocator_type m_allocator;
//	};
//}
//
//
//namespace sic {
//
//	template<ValidComponent ... Cs>
//	void ArchetypeAdmin::PushEntity(Entity const& e)
//	{
//		constexpr size_t entityType{ EntityType<Cs...>() };
//		for (auto&& [compBitset, arch] : m_archetypes)
//		{
//			if (compBitset == entityType)
//			{
//				static_cast<Archetype<Cs...>*>(arch)->PushBack(e.Id());
//				break;
//			}
//		}
//	}
//
//	template<ValidComponent ... Cs>
//	Archetype<Cs...>& ArchetypeAdmin::AddArchetype(size_t compBitset)
//	{
//		Archetype<Cs...>* pArch{ m_allocator.allocate_object<Archetype<Cs...>>() };
//		assert(pArch != nullptr);
//
//		std::construct_at<Archetype<Cs...>>(pArch, m_allocator);
//		m_archetypes.emplace_back(compBitset, pArch);
//		
//		return *static_cast<Archetype<Cs...>*>(pArch);
//	}
//
//	template<ValidComponent...Cs>
//	Archetype<Cs...>& ArchetypeAdmin::GetOrAddArchetype(void)
//	{
//		constexpr size_t components{ EntityType<Cs...>() };
//		for (auto&& [compBitset, arch] : m_archetypes)
//		{
//			if (compBitset == components)
//			{
//				return *static_cast<Archetype<Cs...>*>(arch);
//			}
//		}
//		return AddArchetype<Cs...>(components);
//	}
//
//	template<typename F, ValidComponent ... Cs>
//	requires std::invocable<F&, Cs&...>
//	void ArchetypeAdmin::Each(F fn)
//	{
//		constexpr size_t components{ EntityTypes<Cs...>() };
//		for (auto&& [compBitset, arch] : m_archetypes)
//		{
//			if ((compBitset & components) == components)
//			{
//				// Down cast to a (possibly) different type.... we'll see
//				auto tuple_of_vecs{ static_cast<Archetype<Cs...>*>(arch)->Get<Cs...>() };
//
//				for (size_t i = 0; i < std::get<0>(tuple_of_vecs).size(); ++i)
//				{
//					std::invoke(fn, std::get<std::pmr::vector<Cs>>(tuple_of_vecs)[i]...);
//				}
//
//			}
//		}
//	}
//
//}