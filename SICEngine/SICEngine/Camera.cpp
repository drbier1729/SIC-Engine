/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: - Dylan Bier
 */
 

#include "Camera.h"

#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <SDL_video.h>
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>

#include "Transform.h"
#include "Shape.h"
// TODO : remove these dependencies! Just for HandleInput method...
#include "InputManager.h"
#include "FrameRateController.h"
#include "System.h"


namespace sic
{
	void Camera::ToggleInvertedColors(void)
	{
		m_invert_colors = !m_invert_colors;
		m_clear_color = vec3(1.0f) - m_clear_color;
		glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, 1.0f);
	}
	
	Camera& Camera::RefreshViewFrustum(i32 screen_width, i32 screen_height) 
	{ 
		m_rx = gm_ry * static_cast<float>(screen_width) / screen_height;
		return *this; 
	}


	void Camera::ToggleFullscreen(void) const
	{
		// ref
		bool& is_fullscreen = System::Settings().fullscreen_mode;
		
		if (is_fullscreen)
		{
			SDL_SetWindowFullscreen(System::Window(), 0);
		}
		else
		{

			SDL_SetWindowFullscreen(System::Window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		}

		is_fullscreen = !is_fullscreen;
	}

	void Camera::PrepareToDraw(void)
	{		
		i32& width = System::Settings().screen_width;
		i32& height = System::Settings().screen_height;
		SDL_GetWindowSize(sic::System::Window(), &width, &height);
		RefreshViewFrustum(width, height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set Projection, View, and Inverse matrices
		m_proj = Perspective(m_rx, gm_ry, gm_front_clip_plane, gm_back_clip_plane);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		m_view = LookAt(m_position, m_target, glm::vec3(0.0, 1.0, 0.0));
		m_inverse = glm::inverse(m_view);
	}

	void Camera::FinishDrawing(void)
	{
		SDL_GL_SwapWindow(System::Window());
	}

	void Camera::LoadCamDataToGPU(i32 shaderId)
	{
		int loc = 0;
		
		glm::mat4 view_proj = m_proj * m_view;		
		mat4 proj = glm::ortho(0.0f, static_cast<float>(System::Settings().screen_width), 0.0f, static_cast<float>(System::Settings().screen_height));

		loc = glGetUniformLocation(shaderId, "Camera");
		glUniformMatrix4fv(loc, 1, GL_FALSE, Pntr(view_proj));
		
		loc = glGetUniformLocation(shaderId, "Projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, Pntr(proj));

		loc = glGetUniformLocation(shaderId, "InvertColors");
		glUniform1i(loc, m_invert_colors);

		CHECK_GL_ERROR;
	}

	void Camera::HandleInput(void)
	{
		if (m_debug)
		{
			glm::vec3 move = glm::vec3(0);
			if (System::Input().isKeyPressed(SDL_SCANCODE_UP) || System::Input().isAxisPressedNegative(SDL_CONTROLLER_AXIS_RIGHTY))
			{
				move.y += 1.0f;
			}
			if (System::Input().isKeyPressed(SDL_SCANCODE_DOWN) || System::Input().isAxisPressedPositive(SDL_CONTROLLER_AXIS_RIGHTY))
			{
				move.y -= 1.0f;
			}
			if (System::Input().isKeyPressed(SDL_SCANCODE_LEFT) || System::Input().isAxisPressedNegative(SDL_CONTROLLER_AXIS_RIGHTX))
			{
				move.x -= 1.0f;
			}
			if (System::Input().isKeyPressed(SDL_SCANCODE_RIGHT) || System::Input().isAxisPressedPositive(SDL_CONTROLLER_AXIS_RIGHTX))
			{
				move.x += 1.0f;
			}

			if ((move.x * move.x) + (move.y * move.y) > 1.0f) {
				move *= 0.707f;
			}
			move *= 20.0f * System::FrameRate().DeltaTime();

			m_position += move;
			m_target += move;
		}
	}
}