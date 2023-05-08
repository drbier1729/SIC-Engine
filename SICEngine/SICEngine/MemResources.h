/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Aseem Apastamb
 */
 

#pragma once

#include<memory_resource>
#include "StringId.h"
#include "SICTypes.h"

namespace sic
{
    class DebugMemResource : public std::pmr::memory_resource
    {
    public:
        explicit DebugMemResource(const char* name, std::pmr::memory_resource* up = std::pmr::get_default_resource());

        void* do_allocate(size_t bytes, size_t alignment) override;
        void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;
        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

        u64 DynamicUsed();
        u64 FrameUsed();
        u64 PersistentUsed();

    private:
        const char* _name;
        StringId::underlying_type m_name_hash;
        std::pmr::memory_resource* _upstream;
        u64 m_dyn_used;
        u64 m_fr_used;
        u64 m_per_used;
    };

    class LinearMemResource : public std::pmr::memory_resource
    {
    public:
        explicit LinearMemResource(void* raw_memory, size_t capacity, std::pmr::memory_resource* up = std::pmr::get_default_resource());

        void* do_allocate(size_t bytes, size_t alignment) override;
        inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {}
        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
        void Clear(void);
        
        inline void* top(void) { return m_top; }

    private:
        void* m_base;
        void* m_top;
        size_t m_remaining_space;
        size_t const m_capacity;
        std::pmr::memory_resource* m_upstream;
    };


    // C-style malloc/realloc/free replacements
    
}