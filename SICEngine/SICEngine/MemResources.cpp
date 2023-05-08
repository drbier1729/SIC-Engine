/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Aseem Apastamb
 */
 

#include "MemResources.h"

#include <cstdio>
#include "StringId.h"

namespace sic
{
	DebugMemResource::DebugMemResource(const char* name, std::pmr::memory_resource* up)
		: _name{ name },
		m_name_hash{ StrHash(_name) },
		_upstream{ up },
		m_dyn_used{ 0 },
		m_fr_used{ 0 },
		m_per_used{ 0 }
	{ }

	void* DebugMemResource::do_allocate(size_t bytes, size_t alignment)
	{
		//printf("%s do_allocate(): %llu\n", _name, bytes);
		switch (m_name_hash) {
		case "dynamic"_sid:
			m_dyn_used += bytes;
			break;
		case "frame"_sid:
			m_fr_used += bytes;
			break;
		case "persistent"_sid:
			m_per_used += bytes;
			break;
		default:
			break;
		}
		void* ret = _upstream->allocate(bytes, alignment);
		return ret;
	}

	void DebugMemResource::do_deallocate(void* ptr, size_t bytes, size_t alignment) {
		//printf("%s do_deallocate(): %llu\n", _name, bytes);
		switch (m_name_hash) {
		case "dynamic"_sid:
			m_dyn_used -= bytes;
			break;
		case "frame"_sid:
			m_fr_used -= bytes;
			break;
		case "persistent"_sid:
			m_per_used -= bytes;
			break;
		default:
			break;
		}
		_upstream->deallocate(ptr, bytes, alignment);
	}

	bool DebugMemResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept
	{
		return this == &other;
	}

	u64 DebugMemResource::DynamicUsed() {
		return m_dyn_used;
	}

	u64 DebugMemResource::FrameUsed() {
		return m_fr_used;
	}

	u64 DebugMemResource::PersistentUsed() {
		return m_per_used;
	}


	LinearMemResource::LinearMemResource(void* raw_memory, size_t capacity, std::pmr::memory_resource* up) :
		m_base{ raw_memory },
		m_top{ m_base },
		m_capacity{ capacity },
		m_remaining_space{ capacity },
		m_upstream{ up }
	{ }


	void* LinearMemResource::do_allocate(size_t bytes, size_t alignment)
	{
		void* marker{ m_top };
		if (std::align(alignment, bytes, marker, m_remaining_space))
		{
			m_top = reinterpret_cast<std::byte*>(marker) + bytes;
			m_remaining_space -= bytes;

			return marker;
		}
		return m_upstream->allocate(bytes, alignment);
	}

	bool LinearMemResource::do_is_equal(std::pmr::memory_resource const& other) const noexcept {
		return this == &other;
	}

	void LinearMemResource::Clear(void) {
		m_remaining_space = m_capacity;
		m_top = m_base;
	}
}