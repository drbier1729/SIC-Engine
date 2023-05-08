/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <memory>

#include "SICTypes.h"

struct SDL_Window;
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;
using SDL_GLContext = void*;

struct World;

namespace sic
{
	class Application;
	class ImGuiWindow;
	class MemoryManager;
	class FrameRateController;
	class StringIdDictionary;
	class Camera;
	class InputManager;
	class AudioManager;
	class ResourceManager;
	class EntityAdmin;
	class Factory;
	class EventManager;
	
	struct SystemOptions
	{
		size_t dynamic_memory_size = Megabytes(256);
		size_t persistent_memory_size = Megabytes(128);
		size_t frame_memory_size = Megabytes(64);

		size_t dynamic_memory_blocks_per_chunk = 32;

		i32 screen_width = 800;
		i32 screen_height = 600;

		u32 target_frame_rate = 60;

		i32 gl_major_version = 3;
		i32 gl_minor_version = 3;

		u16 controller_dead_zone = 8000;

		bool mem_dbg_mode = true;

		bool editor_mode = false;

		bool fullscreen_mode = true;
	};

	SystemOptions ConfigSystem(const char* filePath);
}

namespace sic {

	class System
	{
	public:
		// Init and Shutdown
		class_global bool Init(SystemOptions const&);
		class_global void Close(void);


		// Run a game or application
		class_global void        Run(Application* app);
		class_global inline void Quit(void) { gm_running = false; }

		// TODO (dylan) : assert that pointers are not null
		// Accessors
		inline class_global SystemOptions&		 Settings(void)  { return gm_settings; }
		inline class_global SDL_Window*			 Window(void)	 { return gm_main_window; }
		inline class_global SDL_Joystick*		 Controller(void)	 { return gm_game_controller; }
		inline class_global SDL_GLContext	     GLContext(void) { return gm_gl_context; }

		inline class_global Application&		 App(void)   { return *gm_app; }
		inline class_global ImGuiWindow&		 ImGui(void)	{ return *gm_imgui_window; }
		inline class_global MemoryManager&		 Memory(void) { return *gm_mem_mgr; }
		inline class_global FrameRateController& FrameRate(void) { return *gm_frc; }
		inline class_global ResourceManager&	 Resources(void) { return *gm_res_mgr; }
		inline class_global AudioManager&		 Audio(void) { return *gm_audio_mgr; }
		inline class_global InputManager&	     Input(void) { return *gm_input_mgr; }
		inline class_global Camera&     		 Cam(void)	{ return *gm_camera; }
		inline class_global EntityAdmin&		 ECS(void)	{ return *gm_entity_admin; }
		inline class_global EventManager&		 Events(void) { return *gm_event_mgr; }
		inline class_global World&				 Physics(void) { return *gm_physics_world; }
		inline class_global Factory&		     SceneFactory(void)	{ return *gm_factory; }

#ifdef _DEBUG
		inline class_global StringIdDictionary&	 Dictionary(void) { return *gm_sid_dict; }
#endif

	public:
		// This class is pure static and cannot be instantiated
		System() = delete;
		System(System const&) = delete;
		System(System&&) = delete;
		System& operator=(System const&) = delete;
		System& operator=(System&&) = delete;
		~System() = delete;

		
	private:
		// Helpers
		class_global bool Init_SDL_OpenGL(void);
		class_global bool InitManagers(void);
		class_global void CloseManagers(void);

		

	private:
		// System state
		class_global bool gm_initialized;
		class_global bool gm_running;
		class_global SystemOptions gm_settings;

		// SDL and OpenGL
		class_global SDL_Window* gm_main_window;
		class_global SDL_GLContext gm_gl_context;
		
		// Game Controller 1 handler
		class_global SDL_Joystick* gm_game_controller;

		// Game or Application
		class_global Application* gm_app;
		
		// Managers
		class_global std::unique_ptr<MemoryManager> gm_mem_mgr;

		class_global ImGuiWindow* gm_imgui_window;
		class_global FrameRateController* gm_frc;
		class_global Camera* gm_camera;
		class_global ResourceManager* gm_res_mgr;
		class_global InputManager* gm_input_mgr;
		class_global AudioManager* gm_audio_mgr;
		class_global World* gm_physics_world;

		class_global EntityAdmin* gm_entity_admin;
		class_global Factory* gm_factory;
		class_global EventManager* gm_event_mgr;
		
		// Debug Managers
#ifdef _DEBUG
		class_global StringIdDictionary* gm_sid_dict;
#endif

	};
}