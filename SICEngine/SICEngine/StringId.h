/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"
#include <string>
#include <unordered_map>
#include <ostream>

namespace sic {

	// String Hashing
	constexpr u64 val_64_const = 0xcbf29ce484222325;
	constexpr u64 prime_64_const = 0x100000001b3;

	inline constexpr u64 fnv1a_64(const char* const str, const uint64_t value = val_64_const) noexcept
	{
		return (str[0] == '\0') ? value : fnv1a_64(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
	}

	constexpr str_hash_type StrHash(const char* str) { return sic::fnv1a_64(str); }
}


namespace sic {

	class StringIdDictionary;
	class StringId;


#ifdef _DEBUG
	class StringId
	{
	public:
		friend class StringIdDictionary;
		using underlying_type = str_hash_type;

		// Ctor hashes the string. In debug: interns the C-string in a global lookup table.
		explicit StringId(const char* str);
		
		// Conversion from underlying_type
		StringId(underlying_type id);
		StringId& operator=(underlying_type const& id);
		StringId& operator=(underlying_type&& id);

		// Conversion to underlying_type
		inline underlying_type Id(void) const noexcept { return m_id; }
		inline const char* Str(void) const noexcept { return m_string; }

		// Global Setter
		class_global inline void SetStringDictionary(StringIdDictionary* dict) { gm_string_id_table = dict; }

	public:
		StringId() = default;
		StringId(StringId const& src) = default;
		StringId(StringId && src) = default;
		StringId& operator=(StringId const& src) = default;
		StringId& operator=(StringId && src) = default;

		~StringId() = default;

		// Operators
		friend bool operator==(StringId const& a, StringId const& b) noexcept { return a.m_id == b.m_id; }
		friend bool operator==(underlying_type val, StringId const& b) noexcept { return val == b.m_id; }
		friend bool operator==(StringId const& a, underlying_type val) noexcept { return a.m_id == val; }

		friend bool operator!=(StringId const& a, StringId const& b) noexcept { return !(a.m_id == b.m_id); }
		friend bool operator!=(underlying_type val, StringId const& b) noexcept { return !(val == b.m_id); }
		friend bool operator!=(StringId const& a, underlying_type val) noexcept { return !(a.m_id == val); }

		friend std::ostream& operator<<(std::ostream& os, StringId const& sid);

	private:
		underlying_type m_id;
		const char* m_string;

		class_global StringIdDictionary* gm_string_id_table;
	};

#else
	class StringId
	{
	public:
		friend class StringIdDictionary;
		using underlying_type = str_hash_type;

		// Ctor hashes the string.
		explicit constexpr StringId(const char* str) : m_id{ fnv1a_64(str) } {}
		
		// Conversion from underlying_type
		constexpr StringId(underlying_type id) : m_id{ id } {}
		constexpr StringId& operator=(underlying_type const& id) { m_id = id; return *this; }
		constexpr StringId& operator=(underlying_type&& id) { m_id = id; return *this; }

		// Accessor
		inline constexpr underlying_type  Id(void) const noexcept { return m_id; }

	public:
		constexpr StringId() = default;
		constexpr StringId(StringId const& src) = default;
		constexpr StringId(StringId&& src) = default;
		constexpr StringId& operator=(StringId const& src) = default;
		constexpr StringId& operator=(StringId&& src) = default;

		constexpr ~StringId() = default;

		// Operators
		friend constexpr bool operator==(StringId const& a, StringId const& b) noexcept { return a.m_id == b.m_id; }
		friend constexpr bool operator==(underlying_type val, StringId const& b) noexcept { return val == b.m_id; }
		friend constexpr bool operator==(StringId const& a, underlying_type val) noexcept { return a.m_id == val; }

		friend constexpr bool operator!=(StringId const& a, StringId const& b) noexcept { return !(a.m_id == b.m_id); }
		friend constexpr bool operator!=(underlying_type val, StringId const& b) noexcept { return !(val == b.m_id); }
		friend constexpr bool operator!=(StringId const& a, underlying_type val) noexcept { return !(a.m_id == val); }

		friend std::ostream& operator<<(std::ostream& os, StringId const& sid);
	private:
		underlying_type m_id = 0ull;
	};
#endif


	class StringIdDictionary
	{
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;
		using underlying_type = StringId::underlying_type;

		// Ctors + Dtor
		StringIdDictionary();
		explicit StringIdDictionary(allocator_type alloc);
		~StringIdDictionary();

	public:
		const char* StrLookUp(underlying_type id);
		const char* InternString(underlying_type id, const char* str);

	public:
		// Deleted ctors
		StringIdDictionary(StringIdDictionary const& src, allocator_type alloc = {}) = delete;
		StringIdDictionary(StringIdDictionary&&) = delete;
		StringIdDictionary(StringIdDictionary&& src, allocator_type alloc) = delete;
		StringIdDictionary& operator=(StringIdDictionary const& rhs) = delete;
		StringIdDictionary& operator=(StringIdDictionary&& rhs) = delete;


	private:
		std::pmr::unordered_map<underlying_type, std::pmr::string> m_string_id_table;

		class_global bool m_instantiated;
	};


}


// Global Utilities: obtain a compile-time hashed value from a C-string
consteval sic::str_hash_type operator "" _sid(const char* str, std::size_t sz) noexcept 
{ 
	return sic::fnv1a_64(str); 
}