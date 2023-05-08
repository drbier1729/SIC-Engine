/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Andrew Rudasics, Aseem Apastamb, Brian Chen
 */
 

#include "TestGame.h"

#include "SICEngine.h"
#include <iostream>
#include <SDL_scancode.h>

namespace TestGame {

    void TestGame::Initialize(void)
    {    
		currentState = Application::State::Play;
		currentScene = "TestScene.json"_sid;

#ifdef _DEBUG
		sic::System::ImGui().SetCurrentScene(currentScene);
#endif

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
		//sic::System::Events().Subscribe(sic::EventType::Collision, "PlaySoundOnCollisions");
		sic::System::Events().Subscribe(sic::EventType::CollisionEnter, "UpdateCollidingWithOnCollisionEnter");
		sic::System::Events().Subscribe(sic::EventType::CollisionExit, "UpdateCollidingWithOnCollisionExit");
		sic::System::Events().Subscribe(sic::EventType::Destroy, "DestroyEntities");

		// start background music
		//auto sound_clip = sic::System::Resources().LoadAudioFile(sic::System::Audio().SoundSystem(), ".\\Assets\\Audio\\aquarium-loop.mp3");
		//sic::System::Audio().Play(sound_clip, sic::AudioType::MUSIC);
		//sic::System::Audio().LoopSound(".\\Assets\\Audio\\shimmer-loop.wav", sic::AudioType::MUSIC);
		//sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.1f);
		
		// build the first scene
		sic::System::SceneFactory().BuildScene(currentScene);

		// initialize physics world from transform data -- MUST BE CALLED IN INITIALIZE AFTER THE SCENE IS BUILT
		sic::System::ECS().ForEach<sic::Body>(sic::WriteDataToPhysicsWorldSystem);
    }

    void TestGame::Update(sic::f32 dt)
    {
		if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_P))
		{
			currentState = State::Menu;
		}

		// Read data from physics world -- SHOULD BE CALLED EARLY IN THE UPDATE FUNCTION
		sic::System::ECS().ForEach<sic::Body>(sic::ReadDataFromPhysicsWorldSystem);

		// Record player input
		sic::System::ECS().ForEach<sic::PlayerController>(sic::PlayerControllerSystem, dt);

		// Dispatch events and tick the event timer
		sic::System::Events().Dispatch(sic::EventType::CollisionEnter);
		sic::System::Events().Dispatch(sic::EventType::CollisionExit);

		// Patrol system.
		sic::System::ECS().ForEach<sic::Patrol>(sic::PatrolSystem);
		sic::System::ECS().ForEach<sic::Grabber>(sic::GrabSystem);
		sic::System::ECS().ForEach<sic::Grabber>(sic::ThrowSystem);

		// Tag entities that are out of bounds for destruction
		sic::System::ECS().ForEach<sic::Transform>(sic::DestroyOutOfBoundsSystem);

		// Update camera position
		if (sic::System::Cam().Debug() == false)
		{
			sic::System::ECS().ForEach<sic::PlayerController>(sic::FollowCamSystem);
		}

		sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::UpdateFrameSystem);
		sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::PlayerAnimatorControllerSystem);

		// Destroy entities at the end of the update loop
		sic::System::Events().Dispatch(sic::EventType::Destroy);

		// Tick the event queue timer
		sic::System::Events().Tick(dt);
    }

	void TestGame::MenuUpdate(sic::f32 dt)
	{
		if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_P))
		{
			currentState = State::Play;
		}


		// Tick the event queue timer
		sic::System::Events().Tick(dt);

		// Check if mouse is hover on the menu button
		sic::System::ECS().ForEach<sic::Text>(sic::MenuClickSystem);
	}
    
	void TestGame::Render(void)
	{
		sic::System::ECS().ForEach<sic::SpriteRenderer>(sic::DrawSpritesSystem);

		if (sic::System::Cam().DebugCollider())
		{
			sic::System::ECS().ForEach<sic::Body>(sic::DebugDrawCollidersSystem);
		}

		if (currentState == State::Menu)
		{
			// render menu
			//sic::System::ECS().ForEach<sic::FadeScreen>(sic::ScreenFadeSystem, 0.0f);
			sic::System::ECS().ForEach<sic::Text>(sic::DrawTextSystem);
		}
		else
		{
			//sic::System::ECS().ForEach<sic::FadeScreen>(sic::ScreenFadeSystem, 1.0f);
		}
	}

}