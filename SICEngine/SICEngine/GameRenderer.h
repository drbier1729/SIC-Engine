/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"
#include <glm.hpp>

struct SDL_Window;

namespace sic
{
	class Camera;
	class Object;
	class Shape;
	class ShaderProgram;


	class GameRenderer
	{
	public:
		GameRenderer() = default;
		~GameRenderer() = default;
		void InitScene();
		void Draw();

	private:

		// window
		SDL_Window* m_window;

		i32 m_window_width;
		i32 m_window_height;

		// camera
		Camera* m_cam_camera;

		f32 m_cam_rx;
		f32 m_cam_ry;
		f32 m_cam_back;
		f32 m_cam_front;

		// light
		glm::vec3 m_light_pos;
		glm::vec3 m_light_bright;
		glm::vec3 m_light_ambient;

		f32 m_light_spin;
		f32 m_light_tilt;
		f32 m_light_dist;

		// shader
		ShaderProgram* m_shader;

		// object
		Shape* box;
		Shape* box2D;
		Object* root;
		Object* box2D_1;
		Object* box2D_2;
	};

}