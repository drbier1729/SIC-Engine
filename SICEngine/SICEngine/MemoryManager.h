/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <memory>
#include <memory_resource>


// Types of Memory
// -- Persistent Memory:
//		--> a monotonic buffer which is not freed until program shutdown
//		--> RULES: 
//			1) no "resizing" of objects (e.g. vectors should be reserved upfront)

// -- Dynamic Memory:
//		--> unsynchronized pool with a monotonic buffer upstream. used as default_resource during game.
//		--> RULES: 
//			1) no objects larger than "largest_required_pool_block" == dynamic_memory_size / 32

// -- Frame Memory:
//		--> linear resource which is cleared at the start of every frame
//		--> RULES: 
//			1) no objects that need to last more than one frame
//			2) no "resizing" of objects

namespace sic {

	class LinearMemResource;
	class DebugMemResource;

	class MemoryManager
	{
	public:
		MemoryManager();
		~MemoryManager();

		bool Init(void);
		void Close(void);

		// Utilities
		void* Alloc(size_t bytes, size_t alignment, std::pmr::memory_resource* memory);
		void Dealloc(void* ptr, size_t bytes, size_t alignment, std::pmr::memory_resource* memory);
		
		template<typename T, typename ... Args>
		T* Create(std::pmr::memory_resource* stack_memory, Args&&... args)
		{
			void* obj_mem{ Alloc(sizeof(T), alignof(T), stack_memory) };
			if (obj_mem) { return std::construct_at<T>(reinterpret_cast<T*>(obj_mem), std::forward<Args>(args)...); }
			return nullptr;
		}
		
		template<typename T>
		void Destroy(std::pmr::memory_resource* stack_memory, T* ptr)
		{
			std::destroy_at(ptr);
			Dealloc(reinterpret_cast<void*>(ptr), sizeof(T), alignof(T), stack_memory);
		}

		void ClearFrameMemory(void);

		// Accessors
		std::pmr::memory_resource* Frame(void);
		std::pmr::memory_resource* Dynamic(void);
		std::pmr::memory_resource* Persistent(void);
		std::pmr::memory_resource* Debug(void);

	public:
		MemoryManager(MemoryManager const&) = delete;
		MemoryManager(MemoryManager &&) = delete;
		MemoryManager& operator=(MemoryManager const&) = delete;
		MemoryManager& operator=(MemoryManager &&) = delete;

	private:
		static bool m_instantiated;
		
		// Upstream Resource (either Null or New/Delete)
		std::unique_ptr<std::pmr::memory_resource> m_default_upstream;

		// Persistent
		std::unique_ptr<std::byte[]> m_raw_persistent_memory;
		std::unique_ptr<std::pmr::monotonic_buffer_resource> m_persistent_memory;
		std::unique_ptr<sic::DebugMemResource> m_dbg_persistent_memory;

		// Frame
		std::unique_ptr<std::byte[]> m_raw_frame_memory;
		std::unique_ptr<sic::LinearMemResource> m_frame_memory;
		std::unique_ptr<sic::DebugMemResource> m_dbg_frame_memory;

		// Dynamic
		std::unique_ptr<std::byte[]> m_raw_dynamic_memory;
		std::unique_ptr<std::pmr::monotonic_buffer_resource> m_dynamic_memory_upstream;
		std::unique_ptr<sic::DebugMemResource> m_dbg_dynamic_memory_upstream;
		std::unique_ptr<std::pmr::unsynchronized_pool_resource> m_dynamic_memory;
		std::unique_ptr<sic::DebugMemResource> m_dbg_dynamic_memory;

		// Debug (New/Delete)
		std::pmr::memory_resource* m_debug_memory;
		std::unique_ptr<DebugMemResource> m_dbg_debug_memory;
	};
}

