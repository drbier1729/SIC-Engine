/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include "StringId.h"

#include <cstdlib>
#include <string>
#include <cassert>

namespace sic {


	// String Id

#ifdef _DEBUG
	StringIdDictionary* StringId::gm_string_id_table{ nullptr };
#endif


#ifdef _DEBUG
	StringId::StringId(const char* str) 
		: m_id{ StrHash(str) }, 
		m_string{ gm_string_id_table->InternString(m_id, str) }
	{ }

	StringId::StringId(underlying_type id)
		: m_id{ id }, m_string{ gm_string_id_table->StrLookUp(id) }
	{ 
		assert(m_string != nullptr && "String not found in Dictionary.");
	}


	StringId& StringId::operator=(underlying_type const& id)
	{
		m_id = id;
		m_string = gm_string_id_table->StrLookUp(id);
		assert(m_string != nullptr && "String not found in Dictionary.");
		return *this;
	}

	StringId& StringId::operator=(underlying_type&& id)
	{
		m_id = id;
		m_string = gm_string_id_table->StrLookUp(id);
		assert(m_string != nullptr && "String not found in Dictionary.");
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, StringId const& sid) {
		os << "(StringId: " << sid.m_id << ") ";
		os << sid.m_string;
		return os;
	}

#else
	std::ostream& operator<<(std::ostream& os, StringId const& sid) {
		os << "(StringId: " << sid.m_id << ") ";
		return os;
	}
#endif

	// String Id Dictionary

	bool StringIdDictionary::m_instantiated{ false };

	StringIdDictionary::StringIdDictionary()
		: StringIdDictionary(allocator_type{}) 
	{}
	
	StringIdDictionary::StringIdDictionary(allocator_type alloc)
		: m_string_id_table{ alloc }
	{
		assert(!m_instantiated && "This object already has an instance!");
		m_instantiated = true;
	}

	StringIdDictionary::~StringIdDictionary()
	{
		m_string_id_table.clear();
	
		m_instantiated = false;
	}

	const char* StringIdDictionary::InternString(underlying_type id, const char* str)
	{
		auto it{ m_string_id_table.find(id) };
		if (it == m_string_id_table.end())
		{
			// Insert a duplicate of the string into the global table
			m_string_id_table.emplace(id, str);
		}

		// TODO (dylan) : is this okay?
		return m_string_id_table[id].c_str();
	}

	const char* StringIdDictionary::StrLookUp(underlying_type id)
	{
		auto it{ m_string_id_table.find(id) };
		if (it != m_string_id_table.end())
		{
			// TODO (dylan) : is this okay?
			return m_string_id_table[id].c_str();
		}

		return nullptr;
	}
}