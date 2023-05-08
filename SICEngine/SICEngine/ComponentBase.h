/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <vector>
#include <tuple>
#include <bitset>
#include <concepts>
#include <cassert>

#include "externals/plf_colony.h"

#include "SICTypes.h"

// Entity and component constants
namespace sic {

	internal_linkage constexpr u64 MAX_ACTIVE_ENTITIES{ 4096 };

	internal_linkage constexpr u64 ENTITY_ID_MASK{ 0x00000000FFFFFFFF }; // entity indexs are u32
	internal_linkage constexpr u64 ENTITY_GEN_MASK{ 0x7FFFFFFF00000000 }; // generations are i32

	internal_linkage constexpr EntityID NullEntityId{ ENTITY_GEN_MASK | ENTITY_ID_MASK };
}

// Component Base
namespace sic {

	struct ComponentBase
	{
	public:
		friend class  EntityAdmin;
		friend struct Entity;

	public:
		ComponentBase() : owner{ NullEntityId } {};

		//ComponentBase& operator=(ComponentBase const&) { return *this; }
		//ComponentBase& operator=(ComponentBase&&) noexcept { return *this; }

	private:
		EntityID owner;
	};

	// TODO (dylan) : any other constraints on Components?
	template<typename T>
	concept ValidComponent = requires{ T::owner; } && requires { T::id; };
	
	// Helper for accessing component id
	template<ValidComponent C>
	constexpr ComponentID CompId(void) { return C::id; }
}

// Component registration
namespace sic {
	
	// Empty struct created with a list of types
	template <typename ... Ts>
	struct TypeList {};


	// Tuple of Vectors of various types
	template<class> class SOA;

	template<template<typename ... Args> class t, typename ... Ts>
	struct SOA<t<Ts...>>
	{
		using allocator_type = std::pmr::polymorphic_allocator<>;
		using type = std::tuple<std::pmr::vector<Ts>...>;
	};

	template<class> class Hive;

	template<template<typename ... Args> class t, typename ... Ts>
	struct Hive<t<Ts...>>
	{
		using type = std::tuple<plf::colony<Ts, std::pmr::polymorphic_allocator<Ts>>...>;
	};

	// Helpers for visiting (and operating on) SOA and Hive by index -- linear search over tuple for correct index
	namespace detail {

		template <int Idx, class Tuple, class Visitor, std::enable_if_t<(Idx >= std::tuple_size<Tuple>::value), bool> = true>
		void visit_tuple_impl(Tuple& tuple, size_t idx, const Visitor& visitor)
		{}

		template <int Idx, class Tuple, class Visitor, std::enable_if_t<(Idx < std::tuple_size<Tuple>::value), bool> = true>
		void visit_tuple_impl(Tuple& tuple, size_t idx, const Visitor& visitor)
		{
			if (Idx == idx) { visitor(std::get<Idx>(tuple)); }
			else  		    { visit_tuple_impl<Idx + 1>(tuple, idx, visitor); }
		}

		// Requires Func to be invocable on ALL elements of TupleT and take the index as a second arg
		template<typename TupleT, typename Func, size_t ... Idxs>
		void tuple_for_each_idx_except_last_impl(TupleT& tp, Func fn, std::index_sequence<Idxs...>)
		{
			(fn(std::get<Idxs>(tp), Idxs), ...);
		}

	}

	template <class Tuple, class Visitor>
	void visit_tuple(Tuple& tuple, size_t idx, const Visitor& visitor)
	{
		detail::visit_tuple_impl<0>(tuple, idx, visitor);
	}

	template< typename TupleT, typename Func, size_t TupSize = std::tuple_size<TupleT>::value>
	void tuple_for_each_idx(TupleT& tp, Func fn)
	{
		detail::tuple_for_each_idx_except_last_impl(tp, fn, std::make_index_sequence<TupSize-1>{});
	}
}