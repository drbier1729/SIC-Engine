/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include <glew.h>
#include <ext.hpp> 

#include <gl/GLU.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "GameRenderer.h"
#include "System.h"
#include "Shader.h"
#include "Camera.h"
#include "Shape.h"
#include "Object.h"
#include "Transform.h"

#define CHECKERROR {GLenum err = glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "OpenGL error (at line scene.cpp:%d)\n", __LINE__); exit(-1);} }
namespace sic
{
	const float PI = 3.14159f;
	const float rad = PI / 180.0f;



	glm::mat4 Identity = glm::mat4(1);

	void GameRenderer::InitScene()
	{
		// window 
		m_window = System::Window();

		m_window_width = 800;
		m_window_height = 600;

		// Build Shaders
		m_shader = new ShaderProgram(); // warning

		m_shader->AddShader("final.vert",    GL_VERTEX_SHADER);
		m_shader->AddShader("final.frag",    GL_FRAGMENT_SHADER);

		m_shader->AddShader("lighting.vert", GL_VERTEX_SHADER);
		m_shader->AddShader("lighting.frag", GL_FRAGMENT_SHADER);

		glBindAttribLocation(m_shader->programId, 0, "vertex");
		glBindAttribLocation(m_shader->programId, 1, "uv");
		//glBindAttribLocation(m_shader->programId, 2, "vertexTexture");
		//glBindAttribLocation(m_shader->programId, 3, "vertexTangent");

		m_shader->LinkProgram();

		// Build Mesh Object
		box = new Box(); // warning
		box2D = new Box2D();
		CHECKERROR;
		root = new Object(NULL); // warning
		box2D_1 = new Object(box2D, glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 10); // warning
		box2D_2 = new Object(box2D, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 10); // warning
		
		// Create Camera
		//m_cam_camera = new Camera(m_window_width, m_window_height, glm::vec3(0.0, - 25.0f, 0.0f), glm::vec3(0.0f, 30.0f, 0.0f)); // warning

		m_cam_ry = 0.4f;
		m_cam_rx = m_cam_ry * m_window_width / m_window_height;
		m_cam_front = 0.5f;
		m_cam_back = 5000.0f;

		// Create Light
		m_light_bright  = glm::vec3(3.0, 3.0, 3.0);
		m_light_ambient = glm::vec3(0.1, 0.1, 0.1);

		m_light_spin = 150.0;
		m_light_tilt = -45.0;
		m_light_dist = 100.0;

		// Build Transforms
		root->add(box2D_1, Scale(15.0f, 15.0f, 0.0f));
		root->add(box2D_2, Translate(0.0, 0.0, 10.0) * Scale(30.0f, 30.0f, 0.0f));
		CHECKERROR;
	}

	void GameRenderer::Draw()
	{
		// set Viewport
		SDL_GetWindowSize(m_window, &m_window_width, &m_window_height);
		glViewport(0, 0, m_window_width, m_window_height);


		m_light_pos = glm::vec3(m_light_dist * cos(m_light_spin * rad) * sin(m_light_tilt * rad),
								m_light_dist * sin(m_light_spin * rad) * sin(m_light_tilt * rad),
								m_light_dist * cos(m_light_tilt * rad));

		m_cam_rx = m_cam_ry * m_window_width / m_window_height;
		CHECKERROR;
		int loc, programId;

		// Choose the lighting shader
		m_shader->Use();
		programId = m_shader->programId;

		// pass view & perspective & inverse matrix to shader
		//m_cam_camera->UseCam(m_cam_rx, m_cam_ry, m_cam_front, m_cam_back, programId);

		// Pass Values to shader Uniforms
		loc = glGetUniformLocation(programId, "lightPos");
		glUniform3fv(loc, 1, &(m_light_pos[0]));
		loc = glGetUniformLocation(programId, "Light");
		glUniform3fv(loc, 1, &(m_light_bright[0]));
		loc = glGetUniformLocation(programId, "Ambient");
		glUniform3fv(loc, 1, &(m_light_ambient[0]));
		CHECKERROR;

		// draw to screen
		root->Draw(m_shader, Identity);
		

		// ture off shader
		m_shader->Unuse();
	}
}
