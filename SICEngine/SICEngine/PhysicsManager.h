//#pragma once
//
//#include <array>
//
//#include "SICTypes.h"
//#include "MemResources.h"
//#include "Entities.h"
//
//namespace sic {
//
//	struct Entity;
//	class EntityAdmin;
//
//	class PhysicsManager
//	{
//	public:
//		explicit PhysicsManager(EntityAdmin& ecs)
//			: m_ecs{ &ecs }, 
//			m_collisions_buffer{}, 
//			m_collisions_resource{ m_collisions_buffer.data(), gm_buffer_size*sizeof(CollisionInfo) } 
//		{}
//
//		PhysicsManager(PhysicsManager const&) = delete;
//		PhysicsManager(PhysicsManager&&) = delete;
//		PhysicsManager& operator=(PhysicsManager const&) = delete;
//		PhysicsManager& operator=(PhysicsManager&&) = delete;
//
//		~PhysicsManager() = default;
//
//		void Update(f32 dt);
//		
//	public:
//		struct CollisionInfo
//		{
//			CollisionInfo() 
//				: entity_ids{},
//				normal{ vec3(0.0f) }, 
//				penetrationDepth{ 0.0f }, 
//				when{ -1.0f }, 
//				collision{ false }
//			{}
//			EntityID entity_ids[2];
//			vec3 normal;
//			f32 penetrationDepth;
//			f32 when; // If collision: between 0.0f and 1.0f. Else: -1.0f.
//			bool8 collision; // TODO : redundant. we can just set when < 0.0f to indicate no collision this frame.
//		};
//
//	private:
//		void RealignAABBs();
//		void RealignCircles();
//		void RealignGroundCheckers();
//
//		void DetectAABBCollisions();
//		void DetectCircleCollisions();
//		void DetectCircleAABBCollisions();
//		void DetectGroundCollisions();
//
//		void ResolveCollisions(f32 dt);
//		void PositionCorrectionForCollisions(f32 dt);
//
//	private:
//		CollisionInfo AABBtoAABB(vec3 const& Center0, f32 rx0, f32 ry0, f32 rz0,
//								 vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1);
//		
//		CollisionInfo CircleToCircle(vec3 const& Center0, f32 Radius0,
//									 vec3 const& Center1, f32 Radius1);
//		
//		CollisionInfo CircleToAABB(vec3 const& Center0, f32 Radius0,
//								   vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1);
//		
//		bool8 GroundedCheck(vec3 const& Center0, f32 rx0, f32 rz0,
//							vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1);
//
//	private:
//		class_global constexpr vec3 compass[] {
//			vec3(1.0f, 0.0f, 0.0f),		// Right
//			vec3(-1.0f, 0.0f, 0.0f),	// Left
//			vec3(0.0f, 1.0f, 0.0f),		// Up
//			vec3(0.0f, -1.0f, 0.0f),	// Down
//			vec3(0.0f, 0.0f, 1.0f),		// Front
//			vec3(0.0f, 0.0f, -1.0f)		// Back
//		};
//
//		EntityAdmin* m_ecs;
//
//
//		class_global constexpr size_t gm_buffer_size{ 512 };
//		using buffer_type = std::array<CollisionInfo, gm_buffer_size>;
//
//		buffer_type m_collisions_buffer;
//		LinearMemResource m_collisions_resource;
//	};
//
//}