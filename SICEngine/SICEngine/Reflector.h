/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier
 */
 
 // Source: https://technicallydeclined.com/static-reflection-and-serialization-in-c/

#pragma once
#include "SICTypes.h"
#include "StringId.h"
#include <string>
#include <array>

namespace sic {

constexpr u8 MAX_NUM_OF_FIELDS = 20;
#define ALLOW_REFLECT_PRIVATE friend class Reflector;

	class Reflector
	{
	public:
		enum class TypeName {
			u8,
			u16,
			u32,
			u64,
			i8,
			i16,
			i32,
			i64,
			f32,
			f64,
			vec2,
			vec3,
			vec4,
			StringId,
			bool8,
			EntityID
		};

		struct Type {
			const char* stringName;
			TypeName enumName;
			size_t size;
		};

		struct Field {
			Type const * type;
			const char* name;
			size_t offset;
		};

		struct Class {
			std::array<Field, MAX_NUM_OF_FIELDS> fields;
		};

		template<typename T, Reflector::TypeName TEnum>
		class_global Type const * GetType();

		template<typename T>
		class_global Class const * GetClass();

		#define DEFINE_TYPE(TYPE)\
		template<>\
		Reflector::Type const * Reflector::GetType<TYPE, Reflector::TypeName::TYPE>() {\
			local_persistent Reflector::Type type;\
			type.stringName = #TYPE;\
			type.size = sizeof(TYPE);\
			type.enumName = Reflector::TypeName::TYPE;\
			return &type;\
		}

		#define BEGIN_ATTRIBUTES_FOR(CLASS)\
		template<>\
		Reflector::Class const * Reflector::GetClass<CLASS>() {\
			using ClassType = CLASS;\
			local_persistent Reflector::Class localClass;\
			enum {\
				BASE = __COUNTER__\
			};\

			#define DEFINE_MEMBER(TYPE, NAME)\
			enum {\
				NAME##Index = __COUNTER__ - BASE - 1\
			};\
			localClass.fields[NAME##Index].type = Reflector::GetType<decltype(ClassType::NAME), Reflector::TypeName::TYPE>();\
			localClass.fields[NAME##Index].name = #NAME;\
			localClass.fields[NAME##Index].offset = offsetof(ClassType, NAME);\

			#define END_ATTRIBUTES\
			return &localClass;\
		}
	};

}