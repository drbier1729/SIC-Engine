/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Andrew Rudasics, Aseem Apastamb, Brian Chen
 */
 

#pragma once

#include <cassert>
#include <array>

#include "SICTypes.h"
#include "ComponentBase.h"
#include "StringId.h"
#include "Material.h"
#include "Shape.h"
#include "AnimatorController.h"

#include "externals/MathUtils.h"

// Header: Enumerate all registered component types
namespace sic {

	enum class ComponentType : ComponentID
	{
#define REGISTER_COMPONENT(TYPE) TYPE,
#include "Components.def"
#undef REGISTER_COMPONENT
		INVALID
	};

	constexpr u64 NUM_COMPONENTS{ static_cast<u64>(ComponentType::INVALID) };

	// Some macros for defining components
#define ASSIGN_COMPONENT_ID(type) class_global constexpr ComponentID id{ static_cast<ComponentID>(ComponentType::##type) }
#define COMPONENT(name) struct name { EntityID owner = NullEntityId; ASSIGN_COMPONENT_ID(name);
#define COMPONENT_NO_REFL(name) struct name { EntityID owner = NullEntityId; ASSIGN_COMPONENT_ID(name);
#define COMPONENT_END };
}


// Accessors for Component types and names
namespace sic {

	ComponentType ComponentTypeFromName(const char* name);
	ComponentType ComponentTypeFromName(StringId name);
	ComponentType ComponentTypeFromName(StringId::underlying_type name);

	const char* const ComponentNameFromType(ComponentType type);
	const char* const ComponentNameFromId(ComponentID id);
}

// Component Definitions
namespace sic {

	COMPONENT(Name)
		Name() : name{ "<NO NAME>" } {}
		StringId name;
	COMPONENT_END

	COMPONENT(Transform)
	
		// TODO : decide on world axes so rotation can be decided with one float
		Transform()
			: position{ 0.0f, 0.0f, 0.0f, 1.0f },
			rotation{ 0.0f, 0.0f, 0.0f },
			scale{ 1.0f, 1.0f, 1.0f }
		{}

		vec4 position;
		vec3 rotation;
		vec3 scale;

	COMPONENT_END

	COMPONENT(Body)
		Body();
		void CalculateImpulse();
		void UpdatePosAndRot(vec2 const & pos, f32 rot);

		void AddForce(vec2 const & f)
		{
			force += f;
		}

		vec2 position;
		vec2 positionOffset;
		f32 rotation;

		vec2 velocity;
		f32 angularVelocity;

		vec2 force;
		f32 torque;

		vec2 width;
		bool8 forward;

		f32 friction;
		f32 mass, invMass;
		f32 I, invI;
		f32 restitutionCoeff;

		bool8 setWidthManually;
		bool8 isTrigger;
		bool8 enableRotation;
		bool8 isGrounded;
	COMPONENT_END

	COMPONENT(Joint)
		Joint() 
			: body1(0), body2(0),
			P(0.0f, 0.0f),
			biasFactor(0.2f), softness(0.0f)
		{}

		void Set(Body* body1, Body* body2, const vec2& anchor);

		void PreStep(float inv_dt);
		void ApplyImpulse();

		Mat22 M;
		vec2 localAnchor1, localAnchor2;
		vec2 r1, r2;
		vec2 bias;
		vec2 P;		// accumulated impulse
		Body* body1;
		Body* body2;
		f32 biasFactor;
		f32 softness;
	COMPONENT_END

	COMPONENT_NO_REFL(CollidingWith)

		std::array<EntityID, 8> entity_ids = {
			NullEntityId, NullEntityId,
			NullEntityId, NullEntityId,
			NullEntityId, NullEntityId,
			NullEntityId, NullEntityId };
		std::array<StringId, 8> names = {
			StringId("<NONE>"), StringId("<NONE>"),
			StringId("<NONE>"), StringId("<NONE>"),
			StringId("<NONE>"), StringId("<NONE>"),
			StringId("<NONE>"), StringId("<NONE>") };

	COMPONENT_END

	COMPONENT(SpriteRenderer)
	
		SpriteRenderer() = default;

		StringId texture_file = StringId{ ".\\Assets\\Sprites\\default_texture.png" };
		Material material = Material{};
		Shape* shape = nullptr;

	COMPONENT_END

	COMPONENT_NO_REFL(SpriteAnimator)
	
		SpriteAnimator() : frame(0), clip(0), frame_counter(0), playback_speed(2), flip(false) {}

		// Fields...
		i32 frames[64];
		AnimClip clips[16];

		//AnimatorController* ac;
		i32 frame;
		i32 clip;
		i32 frame_counter;
		i32 playback_speed;
		bool8 flip;

	COMPONENT_END

	COMPONENT(AudioSource)
	COMPONENT_END

	COMPONENT(PlayerController)
	
		PlayerController()
			: acceleration{ 50.0f }, max_jump_height{ 15.0f },
			time_to_apex{ 4.0f },
			g{ (2 * max_jump_height) / (time_to_apex * time_to_apex) },
			init_jump_v{ glm::sqrt(2 * g * max_jump_height) },
			elapsed_time{ 0.0f },
			last_grounded_time{ 0.0f },
			can_jump{ false },
			is_crouched{ false },
			is_dashing{ false },
			is_hiding{ false },
			jump_released{ true }
		{}

		f32 acceleration;
		f32 max_jump_height;
		f32 time_to_apex;
		f32 g;
		f32 init_jump_v;
		f32 elapsed_time;
		f32 last_grounded_time;
		bool8 can_jump;
		bool8 is_crouched;
		bool8 is_dashing;
		bool8 is_hiding;
		bool8 jump_released;

	COMPONENT_END

	COMPONENT(Patrol) 
	
		Patrol() 
			: startingPoint(vec3(0.0f)), direction(vec3(0.0f)), 
			  range(0.0f),speed(0.0f), radius(0.0f), acceleration(2.0f), 
			  isDetected(false), mode(false) {}

		vec3 startingPoint;
		vec3 direction;
		f32 range;
		f32 speed;
		f32 radius;
		f32 acceleration;
		bool8 isDetected;
		bool8 mode;

	COMPONENT_END

	COMPONENT(Health)
	
		Health() = default;

		i32 hp = 100;
		i32 max_hp = 100;

	COMPONENT_END

	COMPONENT(Projectile)
	
		Projectile() = default;

		EntityID spawnedBy = NullEntityId;
		i32 damage = 10;

	COMPONENT_END

	COMPONENT(Grabbable)
		bool8 isGrabbed;
	COMPONENT_END

	COMPONENT(Grabber)

		Grabber() :
		grabbed_entity{ NullEntityId },
			grabbing{ false },
			throw_power{ 0.0f }
		{}

		EntityID grabbed_entity;
		bool8 grabbing;
		f32 throw_power;

		COMPONENT_END
	
	COMPONENT(Text)
		StringId text;
		f32 scale = 1.0f;
		vec3 color = vec3(1.0f, 0.0f, 0.0f);
		bool8 IsCast = false;
		EntityID sub_0 = NullEntityId;
		EntityID sub_1 = NullEntityId;
		bool8 active = false;
	COMPONENT_END

	COMPONENT(EnemyTag)
	COMPONENT_END

	COMPONENT(GroundTag)
	COMPONENT_END

	COMPONENT(BackgroundTag)
	COMPONENT_END

	COMPONENT(BouncyBallTag)
	COMPONENT_END

	COMPONENT(SceneChangeTrigger)
		StringId nextScene;
	COMPONENT_END

	COMPONENT(FadeScreen)
		bool8 fadeIn = false;
		bool8 prevSceneTr = false;
		f32 alpha;
		f32 maximum;
		f32 time;
		u32 code_index = 0;
		COMPONENT_END

			COMPONENT(MenuTag)
			COMPONENT_END

			COMPONENT(SubMenuTag)
			COMPONENT_END

	COMPONENT(SplashTag)
		u32 nextLevel;
		bool8 fadeIn = true;
	COMPONENT_END
}

// Footer: Save a type list of all component types
namespace sic {
	
#undef ASSIGN_COMPONENT_ID
#undef COMPONENT
#undef COMPONENT_NO_REFL
#undef COMPONENT_END

	struct INVALID
	{
		class_global constexpr ComponentID id{ NUM_COMPONENTS };
	};

	// TODO (dylan) : this is only for SparseSet ECS. remove if we switch to Archetype ECS.
	using Components = TypeList<
#define REGISTER_COMPONENT(TYPE) TYPE,
#include "Components.def"
#undef REGISTER_COMPONENT
		INVALID
	>;

}
