/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include "MemoryManager.h"

#include <iostream>
#include <cassert>

#include "MemResources.h"
#include "System.h"

#include "MemResources.h"

namespace sic {

	bool MemoryManager::m_instantiated{ false };

	MemoryManager::MemoryManager()
	{
		assert(!m_instantiated && "Memory Manager is already instantiated!");
		m_instantiated = true;
	}
	
	MemoryManager::~MemoryManager()
	{
		m_instantiated = false;
	}

	bool MemoryManager::Init(void)
	{
		// TODO (dylan/aseem) : revise for simplicity, perf, and policy enforcement

		auto settings{ System::Settings() };

		m_default_upstream = std::make_unique<DebugMemResource>("null", std::pmr::null_memory_resource());

		// Persistent Memory Init
		m_raw_persistent_memory = std::make_unique<std::byte[]>(settings.persistent_memory_size);
		if (!m_raw_persistent_memory)
		{
			std::cout << "\nPersistent memory allocation failed" << '\n';
			return false;
		}
		m_persistent_memory = std::make_unique<std::pmr::monotonic_buffer_resource>(m_raw_persistent_memory.get(), settings.persistent_memory_size, m_default_upstream.get());
		if (settings.mem_dbg_mode) { m_dbg_persistent_memory = std::make_unique<DebugMemResource>("persistent", m_persistent_memory.get()); }


		// Frame Memory Init
		m_raw_frame_memory = std::make_unique<std::byte[]>(settings.frame_memory_size);
		if (!m_raw_frame_memory)
		{
			std::cout << "\nFrame memory allocation failed" << '\n';
			return false;
		}
		m_frame_memory = std::make_unique<LinearMemResource>(m_raw_frame_memory.get(), settings.frame_memory_size, m_default_upstream.get());
		if (settings.mem_dbg_mode) { m_dbg_frame_memory = std::make_unique<DebugMemResource>("frame", m_frame_memory.get()); }


		// Dynamic Memory Init
			// upstream mono buffer init
		m_raw_dynamic_memory = std::make_unique<std::byte[]>(settings.dynamic_memory_size);
		if (!m_raw_dynamic_memory)
		{
			std::cout << "\nDynamic memory allocation failed" << '\n';
			return false;
		}
		m_dynamic_memory_upstream = std::make_unique<std::pmr::monotonic_buffer_resource>(m_raw_dynamic_memory.get(), settings.dynamic_memory_size, m_default_upstream.get());
		if (settings.mem_dbg_mode) { m_dbg_dynamic_memory_upstream = std::make_unique <DebugMemResource>("dynamic upstream", m_dynamic_memory_upstream.get()); }

		// pool init
		auto pool_opts = std::pmr::pool_options{
								.max_blocks_per_chunk = settings.dynamic_memory_blocks_per_chunk,
								.largest_required_pool_block = settings.dynamic_memory_size / settings.dynamic_memory_blocks_per_chunk
		};

		if (settings.mem_dbg_mode)
		{
			m_dynamic_memory = std::make_unique<std::pmr::unsynchronized_pool_resource>(pool_opts, m_dbg_dynamic_memory_upstream.get());
			m_dbg_dynamic_memory = std::make_unique<DebugMemResource>("dynamic", m_dynamic_memory.get());
		}
		else
		{
			m_dynamic_memory = std::make_unique<std::pmr::unsynchronized_pool_resource>(pool_opts, m_dynamic_memory_upstream.get());
		}


		// Debug (New/Delete)
		m_debug_memory = std::pmr::new_delete_resource();
		if (settings.mem_dbg_mode)
		{
			m_dbg_debug_memory = std::make_unique<DebugMemResource>("debug", m_debug_memory);
		}


		// Set Dynamic Memory as default mem resource
		if (settings.mem_dbg_mode) { std::pmr::set_default_resource(m_dbg_dynamic_memory.get()); }
		else { std::pmr::set_default_resource(m_dynamic_memory.get()); }

		return true;
	}

	void MemoryManager::Close()
	{
		// Free memory
		std::pmr::set_default_resource(std::pmr::new_delete_resource());

		// dynamic 
		m_dbg_dynamic_memory.reset();
		m_dynamic_memory.reset();
		m_dbg_dynamic_memory_upstream.reset();
		m_dynamic_memory_upstream.reset();
		m_raw_dynamic_memory.reset();

		// frame 
		m_dbg_frame_memory.reset();
		m_frame_memory.reset();
		m_raw_frame_memory.reset();

		// persistent
		m_dbg_persistent_memory.reset();
		m_persistent_memory.reset();
		m_raw_persistent_memory.reset();
	}

	void* MemoryManager::Alloc(size_t bytes, size_t alignment, std::pmr::memory_resource * memory)
	{
		return memory->allocate(bytes, alignment);
	}

	void MemoryManager::Dealloc(void* ptr, size_t bytes, size_t alignment, std::pmr::memory_resource* memory)
	{
		memory->deallocate(ptr, bytes, alignment);
	}

	void MemoryManager::ClearFrameMemory()
	{
		m_frame_memory->Clear();
	}

	std::pmr::memory_resource* MemoryManager::Frame(void)
	{
#ifdef _DEBUG
		bool mem_dbg{ System::Settings().mem_dbg_mode };
		if (mem_dbg) { return m_dbg_frame_memory.get(); }
#endif
		return m_frame_memory.get();
	}

	std::pmr::memory_resource* MemoryManager::Dynamic(void)
	{
#ifdef _DEBUG
		bool mem_dbg{ System::Settings().mem_dbg_mode };
		if (mem_dbg) { return m_dbg_dynamic_memory.get(); }
#endif
		return m_dynamic_memory.get();
	}

	std::pmr::memory_resource* MemoryManager::Persistent(void)
	{
#ifdef _DEBUG
		bool mem_dbg{ System::Settings().mem_dbg_mode };
		if (mem_dbg) { return m_dbg_persistent_memory.get(); }
#endif
		return m_persistent_memory.get();
	}

	std::pmr::memory_resource* MemoryManager::Debug(void)
	{
#ifdef _DEBUG
		bool mem_dbg{ System::Settings().mem_dbg_mode };
		if (mem_dbg) { return m_dbg_debug_memory.get(); }
#endif
		return m_debug_memory;
	}
}