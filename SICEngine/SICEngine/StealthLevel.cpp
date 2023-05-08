/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "StealthLevel.h"

#include "SICEngine.h"

namespace Stealth {
	void StealthLevel::Initialize(void)
	{
		// Load Asset Manifests
		sic::System::Resources().LoadAudioManifest(".\\Assets\\audio_manifest.json");
		sic::System::Resources().LoadTextureManifest(".\\Assets\\texture_manifest.json");
		sic::System::Resources().LoadShaderManifest(".\\Assets\\shader_manifest.json");
		sic::System::Resources().LoadJsonManifest(".\\Assets\\json_manifest.json");

		// init camera
		sic::System::Cam()
			.SetPosition(glm::vec3(0.0f, 0.0f, 25.0f))
			.RefreshViewFrustum(sic::System::Settings().screen_width, sic::System::Settings().screen_height)
			.SetView(sic::Camera::ViewMode::Side)
			.SetDebug(false)
			.SetColliderDebug(false);

		// subscribe event handlers
		sic::System::Events().Subscribe(sic::EventType::Collision, "PlaySoundOnCollisions");
		sic::System::Events().Subscribe(sic::EventType::CollisionEnter, "UpdateCollidingWithOnCollisionEnter");
		sic::System::Events().Subscribe(sic::EventType::CollisionExit, "UpdateCollidingWithOnCollisionExit");
		sic::System::Events().Subscribe(sic::EventType::SceneLoad, "LoadScene");

		// start background music
		//auto sound_clip = sic::System::Resources().LoadAudioFile(sic::System::Audio().SoundSystem(), ".\\Assets\\Audio\\aquarium-loop.mp3");
		//sic::System::Audio().Play(sound_clip, sic::AudioType::MUSIC);
		sic::System::Audio().LoopSound(".\\Assets\\Audio\\shimmer-loop.wav", sic::AudioType::MUSIC);
		sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.1f);
		// build the first scene
		sic::System::SceneFactory().BuildScene("StealthLevel.json");

		// initialize physics world from transform data
		sic::System::ECS().ForEach<sic::Body>(sic::InitializeDataInPhysicsWorldSystem);
	}

	void StealthLevel::Update(sic::f32 dt)
	{
		sic::System::Events().Dispatch(sic::EventType::SceneLoad);

		// Testing player
		{
			// Read data from physics world
			sic::System::ECS().ForEach<sic::Body>(sic::ReadDataFromPhysicsWorldSystem);

			// Record player input
			sic::System::ECS().ForEach<sic::PlayerController>(sic::PlayerControllerSystem, dt);

			// Dispatch events and tick the event timer
			//sic::System::Events().Dispatch(sic::EventType::Collision);
			sic::System::Events().Dispatch(sic::EventType::CollisionEnter);
			sic::System::Events().Dispatch(sic::EventType::CollisionExit);
			sic::System::Events().Tick(dt);

			//// Gravity and Damping
			//sic::System::ECS().ForEach<sic::RigidBody>(sic::UniversalForcesSystem, dt);

			// Patrol system.
			sic::System::ECS().ForEach<sic::Patrol>(sic::PatrolSystem);


			//// Update positions and rotations of all objects
			//sic::System::ECS().ForEach<sic::RigidBody>(sic::CalculatePositionFromVelocitySystem, dt);
			//sic::System::ECS().ForEach<sic::RigidBody>(sic::CalculateRotationFromAngularVelocitySystem, dt);

			// Update camera position
			if (sic::System::Cam().Debug() == false)
			{
				sic::System::ECS().ForEach<sic::PlayerController>(sic::FollowCamSystem);
			}

			sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::UpdateFrameSystem);
			sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::PlayerAnimatorControllerSystem);
		}
	}

	void StealthLevel::Render(void)
	{
		sic::System::ECS().ForEach<sic::SpriteRenderer>(sic::DrawSpritesSystem);
	}

}