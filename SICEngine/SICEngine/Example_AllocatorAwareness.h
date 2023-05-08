/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"
#include "System.h"
#include <memory_resource>
#include <vector>

namespace sic {

	// Not an "AllocatorAware" (i.e. no dynamic memory)
	// -- Use ctor to instantiate on the stack
	// -- Use Create<ExampleNotAA>(/*memory resource*/, /*ctor args*/) if you want to instantiate 
	//    in a particular mem resource
	class ExampleNotAA
	{
	public:
		ExampleNotAA() : a{}, b{} {}

	private:
		i32 a[5];
		f32 b[5];
	};
}


namespace sic {

	// "AllocatorAware" class (i.e. has AA data members)
	// -- use ctor to instantiate on the stack but with all dynamic memory in the appropriate mem resource
	// -- use Create<ExampleAA>(/*stack mem rsrc*/, /*ctor args*/, /*heap mem rsrc*/) if you dare: this allows 
	//    you to place "stack data" in one mem resource and "heap data" in another
	class ExampleAA
	{
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>; // this line is required!!!

		// Default ctors
		ExampleAA() : ExampleAA(allocator_type{}) {}			// This will delegate to AA default ctor with default_memory_resource
		explicit ExampleAA(allocator_type alloc) : v(alloc) {}

		// Copy ctor
		ExampleAA(ExampleAA const& src, allocator_type alloc = {}) : v(src.v, alloc) {}

		// Move ctors
		ExampleAA(ExampleAA&&) = default;
		ExampleAA(ExampleAA&& src, allocator_type alloc) : v(std::move(src.v), alloc) {}

		// Assignment operators
		ExampleAA& operator=(ExampleAA const& rhs) = default;
		ExampleAA& operator=(ExampleAA && rhs) = default;
		
		// User-defined ctor example
		ExampleAA(size_t count, allocator_type alloc = {}) : v(count, alloc) {}

		// Dtor
		~ExampleAA() = default;

	private:
		std::pmr::vector<f32> v;
	};

	// "Rules of Thumb" for creating your own AA class:
	// - Keep user-defined ctors to a minimum and try to avoid default args
	// - pass "allocator_type alloc" as the final arg for ctors
	// - Avoid variadic templates (i.e. template<typename ... Args>) or do your own research first!
	//   There are complications that arise.


	// For more info on creating AA types see:
	// - (1) https://youtu.be/RLezJuqNcEQ?t=1774
	// - (2) https://youtu.be/2LAsqp7UrNs

}