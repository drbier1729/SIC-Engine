/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Dylan Bier, Aseem Apastamb, Brian Chen
 */

#include "PuzzlePrototype.h"

#include "SICEngine.h"
#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <SDL_events.h>

namespace PuzzlePrototype
{
	void PuzzlePrototype::Initialize(void)
	{
		prevState = currentState = State::SceneIn;
		prevScene = currentScene = "SplashLevel1.json"_sid;

#ifdef _DEBUG
		sic::System::ImGui().SetCurrentScene(currentScene);
#endif

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
		//sic::System::Events().Subscribe(sic::EventType::CollisionEnter, "PlaySoundOnCollisions");
		sic::System::Events().Subscribe(sic::EventType::CollisionEnter, "UpdateCollidingWithOnCollisionEnter");
		sic::System::Events().Subscribe(sic::EventType::CollisionExit, "UpdateCollidingWithOnCollisionExit");
		sic::System::Events().Subscribe(sic::EventType::Destroy, "DestroyEntities");

		// start background music
		sic::System::Audio().LoopSound(".\\Assets\\Audio\\looping-guitar.mp3", sic::AudioType::MUSIC);
		sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.2f);
		sic::System::Audio().SetChannelVolume(sic::AudioType::EFFECT, 0.1f);
		
		// build the main menu scene
		sic::System::SceneFactory().BuildScene(currentScene);
	}

	void PuzzlePrototype::SceneIn(sic::f32 dt)
	{
		if (SDL_ShowCursor(-1) == 1)
		{
			SDL_ShowCursor(0);
		}

		// Null Entity -- used to hold a black screen for first frame of the scene
		sic::Entity e{sic::System::ECS()};

		// On the first frame in this state...
		if (currentState != prevState)
		{
			// clear out the old world state
			sic::System::Physics().Clear();
			sic::System::Events().Reset();
			sic::System::ECS().Reset();
	
			// set up a black barrier to obscure the scene loading
			e = sic::System::SceneFactory().BuildEntityFromArchetype("BlankBlackScreen.json"_sid);
			e.Get<sic::Transform>().position.z = 5.0f;

			// build the scene
			sic::System::SceneFactory().BuildScene(currentScene);

			// initialize physics world from transform data
			sic::System::ECS().ForEach<sic::Body>(sic::WriteDataToPhysicsWorldSystem);

			// position the camera at the player
			if (!sic::System::Cam().Debug())
			{
				sic::System::ECS().ForEach<sic::PlayerController>(sic::FollowCamSystem);
			}
		}

		prevState = currentState;

		if (e.IsValid()) { sic::System::ECS().DestroyEntity(e); }

		sic::System::ECS().ForEach<sic::FadeScreen>(sic::CalculateFadeSystem, dt);
	}

	void PuzzlePrototype::Update(sic::f32 dt)
	{
		if (SDL_ShowCursor(-1) == 1)
		{
			SDL_ShowCursor(0);
		}

		prevState = currentState;

		// Update camera position
		if (sic::System::Cam().Debug() == false)
		{
			sic::System::ECS().ForEach<sic::PlayerController>(sic::FollowCamSystem);
		}

		if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_ESCAPE))
		{
			sic::System::SceneFactory().BuildScene("PauseMenuLevel.json"_sid);
			sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.05f);
			sic::System::Audio().SetChannelVolume(sic::AudioType::EFFECT, 0.0f);
			currentState = State::Menu;
		}

		// Read data from physics world -- SHOULD BE CALLED EARLY IN THE UPDATE FUNCTION
		sic::System::ECS().ForEach<sic::Body>(sic::ReadDataFromPhysicsWorldSystem);

		// Record player input
		sic::System::ECS().ForEach<sic::PlayerController>(sic::PlayerControllerSystem, dt);

		// Dispatch events
		sic::System::Events().Dispatch(sic::EventType::CollisionEnter);
		sic::System::Events().Dispatch(sic::EventType::CollisionExit);

		// Behavior
		sic::System::ECS().ForEach<sic::Patrol>(sic::PatrolSystem);
		sic::System::ECS().ForEach<sic::Grabber>(sic::GrabSystem); // TODO : fix
		sic::System::ECS().ForEach<sic::Grabber>(sic::ThrowSystem); // TODO : fix

		// Animation
		sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::UpdateFrameSystem);
		sic::System::ECS().ForEach<sic::SpriteAnimator>(sic::PlayerAnimatorControllerSystem);

		// Destroy entities
		sic::System::ECS().ForEach<sic::Transform>(sic::DestroyOutOfBoundsSystem);
		sic::System::Events().Dispatch(sic::EventType::Destroy);

		// Check to see if we need to switch scenes. If we do, queue an event to load the new one.
		sic::System::ECS().ForEach<sic::CollidingWith>(sic::SceneChangeSystem);
		sic::System::ECS().ForEach<sic::FadeScreen>(sic::CheatCodeSystem);

		// Tick the event timer
		sic::System::Events().Tick(dt);
	}

	void PuzzlePrototype::MenuUpdate(sic::f32 dt)
	{		
		if (SDL_ShowCursor(-1) == 0)
		{
			SDL_ShowCursor(1);
		}

		prevState = currentState;

		// Center menu textures on camera position
		sic::System::ECS().ForEach<sic::MenuTag>(sic::InitMenuSystem);

		// Check if mouse is hover on the menu button
		sic::System::ECS().ForEach<sic::Text>(sic::MenuClickSystem);

		if (currentScene != "MainMenuLevel.json"_sid)
		{
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_ESCAPE))
			{
				prevState = currentState;
				currentState = State::Play;
			}

			// This may have been set by ESCAPE or by a mouse click in MenuClickSystem
			if (currentState != prevState)
			{
				sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.2f);
				sic::System::Audio().SetChannelVolume(sic::AudioType::EFFECT, 0.1f);
				sic::System::ECS().ForEach<sic::MenuTag>(sic::DestroyEntityWith<sic::MenuTag>);
				sic::System::ECS().ForEach<sic::Text>(sic::DestroyEntityWith<sic::Text>);
			}
		}

		sic::System::Events().Dispatch(sic::EventType::Destroy);
		sic::System::Events().Tick(dt);
	}

	void PuzzlePrototype::Render(void)
	{
		sic::System::ECS().ForEach<sic::SpriteRenderer>(sic::DrawSpritesSystem);
		if (sic::System::Cam().DebugCollider())
		{
			sic::System::ECS().ForEach<sic::Body>(sic::DebugDrawCollidersSystem);
		}

		if (currentState == State::Menu)
		{
			sic::System::ECS().ForEach<sic::Text>(sic::DrawTextSystem);
		}

		sic::System::ECS().ForEach<sic::Text>(sic::DrawCastSystem);
		sic::System::ECS().ForEach<sic::FadeScreen>(sic::RenderFadeSystem);
	}

	void PuzzlePrototype::SceneOut(sic::f32 dt)
	{
		prevState = currentState;

		sic::System::ECS().ForEach<sic::FadeScreen>(sic::CalculateFadeSystem, dt);
	}


	void PuzzlePrototype::HandleSDLEvent(SDL_Event& e)
	{
		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_MINIMIZED || e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				if (currentScene != "MainMenuLevel.json"_sid && currentScene != "SplashLevel1.json"_sid)
				{
					sic::System::SceneFactory().BuildScene("PauseMenuLevel.json"_sid);
				}
				sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.0f);
				sic::System::Audio().SetChannelVolume(sic::AudioType::EFFECT, 0.0f);
				currentState = State::Menu;
			}
			else if (e.window.event == SDL_WINDOWEVENT_RESTORED || e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
			{
				if (currentScene != "MainMenuLevel.json"_sid)
				{
					sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.05f);
				}
				else
				{
					sic::System::Audio().SetChannelVolume(sic::AudioType::MUSIC, 0.2f);
				}
			}
		}
	}
}