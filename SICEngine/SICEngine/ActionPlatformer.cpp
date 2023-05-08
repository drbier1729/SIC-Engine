/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 
#include "ActionPlatformer.h"

#include "SICEngine.h"

namespace AP {

	void ActionPlatformer::Initialize(void)
	{
		auto& resources = sic::System::Resources();
		auto& events = sic::System::Events();

		// Load Asset Manifests
		resources.LoadAudioManifest(".\\Assets\\audio_manifest.json");
		resources.LoadTextureManifest(".\\Assets\\texture_manifest.json");
		resources.LoadShaderManifest(".\\Assets\\shader_manifest.json");
		resources.LoadJsonManifest(".\\Assets\\json_manifest.json");

		// init camera
		sic::System::Cam()
			.SetPosition(glm::vec3(0.0f, 0.0f, 25.0f))
			.RefreshViewFrustum(sic::System::Settings().screen_width, sic::System::Settings().screen_height)
			.SetView(sic::Camera::ViewMode::Side)
			.SetDebug(false)
			.SetColliderDebug(false);

		// subscribe event handlers
		events.Subscribe(sic::EventType::Collision, "PlaySoundOnCollisions");
		events.Subscribe(sic::EventType::Collision, "DamageFromCollisions");
		events.Subscribe(sic::EventType::CollisionEnter, "UpdateCollidingWithOnCollisionEnter");
		events.Subscribe(sic::EventType::CollisionExit, "UpdateCollidingWithOnCollisionExit");
		events.Subscribe(sic::EventType::TakeDamage, "CalculateDamage");
		events.Subscribe(sic::EventType::SceneLoad, "LoadScene");
		events.Subscribe(sic::EventType::Shoot, "ShootBall");
		events.Subscribe(sic::EventType::Destroy, "AddDestroyMeTag");

		sic::System::Audio().LoopSound(".\\Assets\\Audio\\shimmer-loop.wav", sic::AudioType::MUSIC);
		sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.1f);

		// build the first scene
		sic::System::SceneFactory().BuildScene("ActionPlatformer_SceneInit.json"_sid);

		// initialize physics world from transform data
		sic::System::ECS().ForEach<sic::Body>(sic::InitializeDataInPhysicsWorldSystem);
	}

	void ActionPlatformer::Update(sic::f32 dt)
	{
		auto& ecs = sic::System::ECS();
		auto& events = sic::System::Events();

		// If we need to load a new scene, do it
		events.Dispatch(sic::EventType::SceneLoad);

		{
			// Read data from physics world
			sic::System::ECS().ForEach<sic::Body>(sic::ReadDataFromPhysicsWorldSystem);

			// Record player input
			ecs.ForEach<sic::PlayerController>(sic::PlayerControllerSystem, dt);

			// Dispatch events and tick the event timer
			//events.Dispatch(sic::EventType::Collision);
			events.Dispatch(sic::EventType::CollisionEnter);
			events.Dispatch(sic::EventType::CollisionExit);
			events.Dispatch(sic::EventType::Shoot);
			events.Dispatch(sic::EventType::TakeDamage);
			events.Dispatch(sic::EventType::Destroy);
			events.Tick(dt);

			//// Gravity and Damping
			//ecs.ForEach<sic::RigidBody>(sic::UniversalForcesSystem, dt);

			// Patrol system
			ecs.ForEach<sic::Patrol>(sic::PatrolSystem);

			//// Update positions and rotations of all objects
			//ecs.ForEach<sic::RigidBody>(sic::CalculatePositionFromVelocitySystem, dt);
			//ecs.ForEach<sic::RigidBody>(sic::CalculateRotationFromAngularVelocitySystem, dt);

			// Tag entities that are out of bounds for destruction
			ecs.ForEach<sic::Transform>(sic::DestroyOutOfBoundsSystem);

			// Update camera position
			if (sic::System::Cam().Debug() == false)
			{
				ecs.ForEach<sic::PlayerController>(sic::FollowCamSystem);
			}

			// Handle animations
			ecs.ForEach<sic::SpriteAnimator>(sic::UpdateFrameSystem);
			ecs.ForEach<sic::SpriteAnimator>(sic::PlayerAnimatorControllerSystem);
		}

		// Destroy entities at the end of the update loop
		ecs.ForEach<sic::DestroyMeTag>(sic::CleanUpCorpsesSystem);
	}

	void ActionPlatformer::Render(void)
	{
		sic::System::ECS().ForEach<sic::SpriteRenderer>(sic::DrawSpritesSystem);
	}

}