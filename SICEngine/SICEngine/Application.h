/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - 
 */
 
#pragma once

#include "SICTypes.h"
#include "StringId.h"

typedef union SDL_Event SDL_Event;

namespace sic {

	class Application
	{
	public:
		Application() = default;
		~Application() = default;

		virtual void Initialize() {}
		virtual void Shutdown() {}

		virtual void SceneIn(f32 dt) {}
		virtual void SceneOut(f32 dt) {}

		virtual void Update(f32 dt) {}
		virtual void MenuUpdate(f32 dt) {}
		virtual void Render() {}
		virtual void ImGuiRender() {}

		virtual void HandleSDLEvent(SDL_Event& e) {}

		enum class State
		{
			Play,
			Menu,
			SceneIn,
			SceneOut
		};

		State currentState;
		State prevState;
		StringId::underlying_type currentScene;
		StringId::underlying_type prevScene;
	};
}

