/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#ifndef GLM_FORCE_INLINE
#define GLM_FORCE_INLINE
#endif

#ifndef GLM_FORCE_SWIZZLE
#define GLM_FORCE_SWIZZLE
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm.hpp>

#include <cstdint>

namespace sic {
	using u64 = uint64_t;
	using u32 = uint32_t;
	using u16 = uint16_t;
	using u8 = uint8_t;

	using i64 = int64_t;
	using i32 = int32_t;
	using i16 = int16_t;
	using i8 = int8_t;

	using f32 = float;
	using f64 = double;
	
	using bool8 = bool;

	using EntityID = uint64_t;
	using ComponentID = uint64_t;
	using EventID = uint8_t;

	using str_hash_type = u64;

	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	using vec4 = glm::vec4;

	using mat2 = glm::mat2;
	using mat3 = glm::mat3;
	using mat4 = glm::mat4;
}

#define internal_linkage static
#define local_persistent static
#define class_global static

constexpr size_t Kilobytes(size_t val) { return (val) * 1024; }
constexpr size_t Megabytes(size_t val) { return (val) * 1024 * 1024; }
constexpr size_t Gigabytes(size_t val) { return (val) * 1024 * 1024 * 1024; }

#define CHECK_GL_ERROR { GLenum err = glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "OpenGL error %d at line %s.cpp:%d\n", err, __FILE__, __LINE__); exit(-1);} }

