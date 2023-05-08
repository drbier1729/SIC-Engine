/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Andrew Rudasics, Aseem Apastamb, Brian Chen
 */
 

#include "Components.h"
#include "Reflector.h"

namespace sic {

	BEGIN_ATTRIBUTES_FOR(Name)
		DEFINE_MEMBER(StringId, name)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Transform)
		DEFINE_MEMBER(vec4, position)
		DEFINE_MEMBER(vec3, rotation)
		DEFINE_MEMBER(vec3, scale)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Body)
		DEFINE_MEMBER(bool8, isTrigger)
		DEFINE_MEMBER(bool8, enableRotation)
		DEFINE_MEMBER(f32, mass)
		DEFINE_MEMBER(vec2, width)
		DEFINE_MEMBER(bool8, setWidthManually)
		DEFINE_MEMBER(f32, restitutionCoeff)

		DEFINE_MEMBER(bool8, forward)
		DEFINE_MEMBER(vec2, position)
		DEFINE_MEMBER(vec2, positionOffset)
		DEFINE_MEMBER(f32, rotation)
		DEFINE_MEMBER(vec2, velocity)
		DEFINE_MEMBER(f32, angularVelocity)
		DEFINE_MEMBER(vec2, force)
		DEFINE_MEMBER(f32, torque)
		DEFINE_MEMBER(f32, friction)
		DEFINE_MEMBER(f32, invMass)
		DEFINE_MEMBER(f32, I)
		DEFINE_MEMBER(f32, invI)
		DEFINE_MEMBER(bool8, isGrounded)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Joint)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(CollidingWith)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(SpriteRenderer)
		DEFINE_MEMBER(StringId, texture_file)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(SpriteAnimator)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(AudioSource)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(PlayerController)
		DEFINE_MEMBER(f32, acceleration)
		DEFINE_MEMBER(f32, max_jump_height)
		DEFINE_MEMBER(f32, time_to_apex)
		DEFINE_MEMBER(f32, g)
		DEFINE_MEMBER(f32, init_jump_v)
		DEFINE_MEMBER(f32, elapsed_time)
		DEFINE_MEMBER(f32, last_grounded_time)
		DEFINE_MEMBER(bool8, can_jump)
		DEFINE_MEMBER(bool8, is_dashing)
		DEFINE_MEMBER(bool8, is_hiding)
		DEFINE_MEMBER(bool8, jump_released)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Patrol)
		DEFINE_MEMBER(vec3, startingPoint)
		DEFINE_MEMBER(vec3, direction)
		DEFINE_MEMBER(f32, range)
		DEFINE_MEMBER(f32, speed)
		DEFINE_MEMBER(f32, radius)
		DEFINE_MEMBER(f32, acceleration)
		DEFINE_MEMBER(bool8, isDetected)
		DEFINE_MEMBER(bool8, mode)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Health)
		DEFINE_MEMBER(i32, hp)
		DEFINE_MEMBER(i32, max_hp)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Projectile)
		DEFINE_MEMBER(EntityID, spawnedBy)
		DEFINE_MEMBER(i32, damage)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(EnemyTag)
	END_ATTRIBUTES
	
	BEGIN_ATTRIBUTES_FOR(GroundTag)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(BouncyBallTag)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(BackgroundTag)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(SceneChangeTrigger)
		DEFINE_MEMBER(StringId, nextScene)
	END_ATTRIBUTES
	
		BEGIN_ATTRIBUTES_FOR(Grabbable)
		DEFINE_MEMBER(bool8, isGrabbed);	
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Grabber)
		DEFINE_MEMBER(EntityID, grabbed_entity)
		DEFINE_MEMBER(bool8, grabbing)
		DEFINE_MEMBER(f32, throw_power)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(Text)
		DEFINE_MEMBER(StringId, text)
		DEFINE_MEMBER(f32, scale)
		DEFINE_MEMBER(vec3, color)
		DEFINE_MEMBER(bool8, IsCast)
		DEFINE_MEMBER(EntityID, sub_0)
		DEFINE_MEMBER(EntityID, sub_1)
	    DEFINE_MEMBER(bool8, active)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(FadeScreen)
		DEFINE_MEMBER(bool8, fadeIn)
		DEFINE_MEMBER(bool8, prevSceneTr)
		DEFINE_MEMBER(f32, alpha)
		DEFINE_MEMBER(f32, maximum)
		DEFINE_MEMBER(f32, time)
		DEFINE_MEMBER(u32, code_index)
	END_ATTRIBUTES

	BEGIN_ATTRIBUTES_FOR(MenuTag)
	END_ATTRIBUTES		
	
	BEGIN_ATTRIBUTES_FOR(SubMenuTag)
	END_ATTRIBUTES	

	BEGIN_ATTRIBUTES_FOR(SplashTag)
		DEFINE_MEMBER(u32, nextLevel)
		DEFINE_MEMBER(bool8, fadeIn)
	END_ATTRIBUTES
}

namespace sic {

	// Component names as string literals, indexed by component id
	internal_linkage constexpr std::array<char const*, NUM_COMPONENTS+1> component_names =
	{
#define REGISTER_COMPONENT(TYPE) #TYPE,
#include "Components.def"
#undef REGISTER_COMPONENT
		"INVALID"
	};

	// Map: ComponentName_sid -> ComponentType
	internal_linkage const std::unordered_map<StringId::underlying_type, ComponentType> component_type_map =
	{
#define REGISTER_COMPONENT(TYPE) { StrHash(#TYPE), ComponentType::##TYPE }, 
#include "Components.def"
#undef REGISTER_COMPONENT
		{ "INVALID"_sid, ComponentType::INVALID }
	};

	ComponentType ComponentTypeFromName(StringId::underlying_type name)
	{
		auto it = component_type_map.find(name);
		if (it != component_type_map.end())
		{
			return component_type_map.at(name);
		}
		return ComponentType::INVALID;
	}

	const char* const ComponentNameFromId(ComponentID id) 
	{ 
		if (id < NUM_COMPONENTS)
		{
			return component_names[id];
		}
		return component_names[NUM_COMPONENTS];		// "INVALID"
	}

	const char* const ComponentNameFromType(ComponentType type) { return ComponentNameFromId(static_cast<ComponentID>(type)); }
	ComponentType ComponentTypeFromName(const char* name) { StringId name_sid{ name }; return ComponentTypeFromName(name_sid.Id()); }
	ComponentType ComponentTypeFromName(StringId name) { return ComponentTypeFromName(name.Id()); }
}