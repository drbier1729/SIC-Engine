/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Aseem Apastamb
 */
 

#include "System.h"

#define NOMINMAX
#include <windows.h>

#include <iostream>
#include <cassert>
#include <algorithm>

#include <glew.h>

#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_Joystick.h>
#include <filereadstream.h> // rapidjson
#include <document.h> // rapidjson

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif 

#include "externals/World.h"

// SICEngine
#include "Application.h"
#include "MemoryManager.h"
#include "ImGuiWindow.h"
#include "FrameRateController.h"
#include "Camera.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "Entities.h"
#include "EventManager.h"
#include "Factory.h"
#include "AudioManager.h"
#include "StringId.h"
#include "ComponentSystems.h"

namespace sic {
	
	SystemOptions ConfigSystem(const char* filePath)
	{
		FILE* fp{ nullptr };
		fopen_s(&fp, filePath, "r");
		assert("File failed to open." && fp);

		// Make sure file size is less than buffer size

		size_t constexpr buf_size{ Kilobytes(4) };
		char readBuffer[buf_size];
		assert("Buffer alloc failed." && readBuffer);

		rapidjson::FileReadStream is(fp, readBuffer, buf_size);

		rapidjson::Document d;
		d.ParseStream(is);
		assert(d.IsObject());

		fclose(fp);
		
		SystemOptions opts;

		auto itr = d.MemberBegin();
		auto end = d.MemberEnd();

		itr = d.FindMember("height");
		if (itr != end)
		{
			opts.screen_height = itr->value.GetInt();
		}
		
		itr = d.FindMember("width");
		if (itr != end)
		{
			opts.screen_width = itr->value.GetInt();
		}

		itr = d.FindMember("target frame rate");
		if (itr != end)
		{
			opts.target_frame_rate = itr->value.GetUint();
		}
		
		itr = d.FindMember("dynamic memory size MB");
		if (itr != end)
		{
			opts.dynamic_memory_size = Megabytes(itr->value.GetUint());
		}
		
		itr = d.FindMember("frame memory size MB");
		if (itr != end)
		{
			opts.frame_memory_size = Megabytes(itr->value.GetUint());
		}
		
		itr = d.FindMember("persistent memory size MB");
		if (itr != end)
		{
			opts.persistent_memory_size = Megabytes(itr->value.GetUint());
		}
		
		itr = d.FindMember("controller dead zone");
		if (itr != end)
		{
			opts.controller_dead_zone = itr->value.GetUint();
		}
		
#ifdef _DEBUG
		itr = d.FindMember("memory debug");
		if (itr != end)
		{
			opts.mem_dbg_mode = itr->value.GetBool();
		}
		
		itr = d.FindMember("editor mode");
		if (itr != end)
		{
			opts.editor_mode = itr->value.GetBool();
		}

		itr = d.FindMember("fullscreen mode");
		if (itr != end)
		{
			opts.fullscreen_mode = itr->value.GetBool();
		}
#else
		opts.editor_mode = false;
		opts.mem_dbg_mode = false;
		opts.fullscreen_mode = true;
#endif
		return opts;
	}
}


// ====================
// System Global Fields
// ====================
namespace sic
{
	bool System::gm_initialized{ false };
	bool System::gm_running{ false };
	SystemOptions System::gm_settings{};

	SDL_Window* System::gm_main_window{ nullptr };
	SDL_Joystick* System::gm_game_controller{ nullptr };
	SDL_GLContext System::gm_gl_context{ nullptr };

	// Application that the engine is running
	Application* System::gm_app{ nullptr };

	// Core Managers
	std::unique_ptr<MemoryManager> System::gm_mem_mgr{ nullptr };

	ImGuiWindow* System::gm_imgui_window{ nullptr };
	FrameRateController* System::gm_frc{ nullptr };
	ResourceManager* System::gm_res_mgr{ nullptr };
	InputManager* System::gm_input_mgr{ nullptr };
	Camera* System::gm_camera{ nullptr };
	AudioManager* System::gm_audio_mgr{ nullptr };
	//PhysicsManager* System::gm_physics_mgr{ nullptr };
	World* System::gm_physics_world{ nullptr };

	// Application Data/Logic
	EntityAdmin* System::gm_entity_admin{ nullptr };
	Factory* System::gm_factory{ nullptr };
	EventManager* System::gm_event_mgr{ nullptr };

	// Debug
#ifdef _DEBUG
	StringIdDictionary* System::gm_sid_dict{ nullptr };
#endif
}

// ==============
// System Methods
// ==============
namespace sic {

	/// <summary>
	/// Initializes SDL, SDL_video, OpenGL and initializes and creates all managers
	/// </summary>
	/// <param name="options:">SystemOptions struct.</param>
	/// <returns> true if all subsystems successfully initialize </returns>
	bool System::Init(SystemOptions const& options)
	{
		assert(!gm_initialized && "System has already been initialized!");

		gm_settings = options;
		gm_initialized = true;

		if (gm_initialized)
		{
			gm_initialized = Init_SDL_OpenGL();
		}

		if (gm_initialized)
		{
			gm_initialized = InitManagers();
		}

		return gm_initialized;
	}

	/// <summary>
	/// Shuts down all core subsystems and frees all memory.
	/// </summary>
	/// <param name="">none</param>
	void System::Close(void)
	{
		assert(gm_initialized && "System was never initialized!");
		gm_initialized = false;

		// Shutdown managers
		CloseManagers();

		// Shutdown SDL
		if (gm_main_window)
		{
			// Close game controller
			SDL_JoystickClose(gm_game_controller);
			gm_game_controller = NULL;

			SDL_DestroyWindow(gm_main_window);
			gm_main_window = nullptr;
		}
		SDL_Quit();
	}
	// ========
	// Run Game
	// ========
	void System::Run(Application* app)
	{
		// Set ref to game and initialize
		gm_app = app;
		gm_app->Initialize();

		// Game loop
		gm_running = true;
		while (gm_running)
		{
			// Handle System Input
			// ------------------------------
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT) 
				{
					Quit();
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
					gm_app->HandleSDLEvent(e);
				}
				else if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.scancode)
					{
					case SDL_SCANCODE_BACKSPACE:
					{
						Quit();
						break;
					}
					case SDL_SCANCODE_F11:
					{
						gm_camera->ToggleFullscreen();
						break;
					}
					case SDL_SCANCODE_M:
					{
						gm_audio_mgr->ToggleChannelMute(sic::AudioType::MUSIC);
						break;
					}
					case SDL_SCANCODE_GRAVE:
					{
						#ifdef _DEBUG
						gm_settings.editor_mode = !gm_settings.editor_mode;
						if (gm_settings.editor_mode == false)
						{
							ECS().ForEach<Body>(WriteDataToPhysicsWorldSystem);
						}
						#endif
						break;
					}
					default: { break; }
					}
				}
				gm_imgui_window->HandleSDLEvent(e);
			}

			gm_input_mgr->Update();
			gm_camera->HandleInput();
			// ------------------------------

			// Game Update
			// ------------------------------
			gm_mem_mgr->ClearFrameMemory();
			gm_frc->FrameStart();
			if (!gm_settings.editor_mode)
			{
				auto dt = gm_frc->DeltaTime();

				switch (gm_app->currentState)
				{
				case Application::State::SceneIn:
				{
					gm_app->SceneIn(dt);
					break;
				}
				case Application::State::Play:
				{
					gm_physics_world->Step(dt);
					gm_app->Update(dt);
					break;
				}
				case Application::State::Menu:
				{
					gm_app->MenuUpdate(dt);
					break;
				}
				case Application::State::SceneOut:
				{
					gm_app->SceneOut(dt);
					break;
				}
				default: { break; }
				}
				gm_audio_mgr->Update();
			}
			// ------------------------------
			
			// Render
			// ------------------------------
			gm_camera->PrepareToDraw();
			gm_app->Render();
			
			gm_frc->FrameEnd();
			
			gm_imgui_window->BeginRender();
			gm_app->ImGuiRender();
			gm_imgui_window->Tools();
			if (gm_settings.editor_mode) { gm_imgui_window->SceneEditor(); }
			gm_imgui_window->EndRender();

			gm_camera->FinishDrawing();
			// ------------------------------
		}

		gm_app->Shutdown();
	}


	// ===============
	// Private helpers
	// ===============

	bool System::Init_SDL_OpenGL(void)
	{
		bool success{ true };
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
		{
			printf("SDL Video or SDL Joystick failed to initialize. SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Set OpenGL version
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gm_settings.gl_major_version);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gm_settings.gl_minor_version);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			// Set texture filtering to linear
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
				printf("Warning: Linear texture filtering not enabled!\n");
			}

			// Check for joysticks
			if (SDL_NumJoysticks() < 1) {
				printf("Warning: No joysticks connected!\n");
			}
			else {
				// Load joystick
				gm_game_controller = SDL_JoystickOpen(0);
				if (gm_game_controller == NULL) {
					printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
				}
			}

			// Create Window
			auto sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
			if (gm_settings.fullscreen_mode) { sdl_window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; }

			gm_main_window = SDL_CreateWindow("Lights Out",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				gm_settings.screen_width, gm_settings.screen_height, 
				sdl_window_flags);

			if (gm_main_window == nullptr)
			{
				printf("SDL failed to create Window. SDL Error %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Create OpenGl context in the Window
				gm_gl_context = SDL_GL_CreateContext(gm_main_window);
				if (gm_gl_context == nullptr)
				{
					printf("OpenGL context could not be created. SDL Error %s\n", SDL_GetError());
					success = false;
				}
				else
				{
					// Init GLEW
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if (glewError != GLEW_OK)
					{
						printf("Warning: error initializing GLEW. %s\n", glewGetErrorString(glewError));
					}

					// OpenGL Settings
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LESS);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glViewport(0, 0, gm_settings.screen_width, gm_settings.screen_height);
					glClearColor(0.0, 0.0, 0.0, 1.0);

					// Init SDL_Image
					i32 imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
					if (!(IMG_Init(imgFlags) & imgFlags))
					{
						printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
						success = false;
					}
				}
			}
		}
		return success;
	}

	bool System::InitManagers(void)
	{
		bool success{ true };

		// MemoryManager
		gm_mem_mgr = std::make_unique<MemoryManager>();
		success = gm_mem_mgr->Init();

		// String Id Dictionary
#ifdef _DEBUG
		gm_sid_dict = gm_mem_mgr->Create<StringIdDictionary>(gm_mem_mgr->Persistent(), gm_mem_mgr->Debug());
		StringId::SetStringDictionary(gm_sid_dict);
#endif

		// Resource Manager
		gm_res_mgr = gm_mem_mgr->Create<ResourceManager>(gm_mem_mgr->Persistent(), gm_mem_mgr->Dynamic());

		// ImGui
		gm_imgui_window = gm_mem_mgr->Create<ImGuiWindow>(gm_mem_mgr->Persistent());
		gm_imgui_window->Create();

		// FrameRateController
		gm_frc = gm_mem_mgr->Create<FrameRateController>(gm_mem_mgr->Persistent(), gm_settings.target_frame_rate);

		// Input Manager
		gm_input_mgr = gm_mem_mgr->Create<InputManager>(gm_mem_mgr->Persistent());

		// Audio Manager
		gm_audio_mgr = gm_mem_mgr->Create<AudioManager>(gm_mem_mgr->Persistent());

		// Camera
		gm_camera = gm_mem_mgr->Create<Camera>(gm_mem_mgr->Persistent());

		// EntityAdmin
		gm_entity_admin = gm_mem_mgr->Create<EntityAdmin>(gm_mem_mgr->Persistent(), gm_mem_mgr->Dynamic());
		
		// Event Manager
		gm_event_mgr = gm_mem_mgr->Create<EventManager>(gm_mem_mgr->Persistent(), gm_mem_mgr->Dynamic());
		
		// Factory
		gm_factory = gm_mem_mgr->Create<Factory>(gm_mem_mgr->Persistent(), *gm_entity_admin, gm_mem_mgr->Dynamic());

		// Physics Manager														// gravity,			iterations per time step	
		gm_physics_world = gm_mem_mgr->Create<World>(gm_mem_mgr->Persistent(),	vec2(0.0f, -70.0f),	10,							gm_mem_mgr->Dynamic());
		//gm_physics_mgr = gm_mem_mgr->Create<PhysicsManager>(gm_mem_mgr->Persistent(), *gm_entity_admin);

		return success; // TODO (dylan) : check to make sure all managers are not nullptr
	}

	void System::CloseManagers(void)
	{
		//Shutdown Debug
#ifdef _DEBUG
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_sid_dict);
#endif

		// Shutdown Managers
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_factory);
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_event_mgr);
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_entity_admin);
		
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_audio_mgr);
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_camera);
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_input_mgr);
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_frc);
		//gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_physics_mgr);

		gm_imgui_window->Destroy();
		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_imgui_window);

		gm_mem_mgr->Destroy(gm_mem_mgr->Persistent(), gm_res_mgr);

		gm_mem_mgr->Close();
		gm_mem_mgr.reset();
	}
}