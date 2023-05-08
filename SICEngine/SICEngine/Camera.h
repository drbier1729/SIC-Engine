/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: - Aseem Apastamb, Dylan Bier
 */
 

#pragma once

#include "SICTypes.h"

namespace sic 
{
	class Camera
	{
	public:
		enum class ViewMode : u8
		{
			Iso,
			Side
		};

	public:
		Camera() = default;

		// Methods
		void HandleInput(void); // TODO : move this elsewhere to eliminate dependency on Input Manager and FRC
		
		void PrepareToDraw(void);
		void FinishDrawing(void);

		void LoadCamDataToGPU(i32 shaderId);

		void ToggleFullscreen(void) const;

		void ToggleInvertedColors(void);

		inline
		void ToggleDebugCam(void) { m_debug = !m_debug; }

		inline
		void ToggleCollider(void) { m_collider_debug = !m_collider_debug; }

		// Accessors
		inline
		ViewMode View(void) const { return m_mode; }

		inline
		bool Debug(void) const { return m_debug; }

		inline
		bool DebugCollider(void) const { return m_collider_debug; }

		inline
		glm::vec3 GetPosition() { return m_position; }

		inline
		mat4 GetViewMatrix() { return m_view; }

		inline
		mat4 GetProjectionMatrix() { return m_proj; }


		// Setters
		Camera& RefreshViewFrustum(i32 screen_width, i32 screen_height);

		inline
		Camera& SetDebug(bool on_off) { m_debug = on_off;	return *this; }

		inline
		Camera& SetColliderDebug(bool on_off) { m_collider_debug = on_off;	return *this; }

		inline
		Camera& SetView(ViewMode new_mode) { m_mode = new_mode;	return *this; }
	
		inline
		Camera& SetPosition(glm::vec3 new_position) { m_position = new_position; return *this; }
		
		inline
		Camera& SetRotation(glm::vec3 new_rotation) { m_rotation = new_rotation; return *this; }

		inline
		Camera& SetTarget(glm::vec3 new_target) { m_target = new_target; return *this; }

	private:
		f32 m_rx;

		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_target;

		glm::mat4 m_view;
		glm::mat4 m_proj;
		glm::mat4 m_inverse;

		glm::vec3 m_clear_color = glm::vec4(0.0f);
		bool m_invert_colors;

		ViewMode m_mode;
		bool m_debug;
		bool m_collider_debug;

		class_global f32 constexpr gm_ry{ 0.4f };
		class_global f32 constexpr gm_front_clip_plane{ 0.5f };
		class_global f32 constexpr gm_back_clip_plane{ 5000.0f };
	};

}

