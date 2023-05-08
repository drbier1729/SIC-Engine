/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 
#pragma once
#include "Application.h"

namespace AP
{
	class ActionPlatformer : public sic::Application
	{
	public:
		ActionPlatformer() = default;
		~ActionPlatformer() = default;

		virtual void Initialize(void) override;
		virtual void Shutdown(void) override {}
		virtual void Update(sic::f32 dt) override;
		virtual void Render(void) override;
		virtual void ImGuiRender(void) override {}
	};
}
