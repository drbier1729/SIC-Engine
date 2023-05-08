/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: -
 */
 

#pragma once

#include <string>
#include <vector>
#include <glew.h>

namespace sic
{
	class Material
	{
	public:
		Material();
		Material(GLuint tex, GLuint shader);
		Material(GLuint tex, GLuint shader, float tile_x, float tile_y);
		Material(GLuint tex, GLuint shader, float tile_x, float tile_y, int rows, int cols);
		~Material();
		GLuint getShader();
		void Use();
		void Unuse();
		
		inline GLfloat GetX(void) const { return m_tile_x; }
		inline GLfloat GetY(void) const { return m_tile_y; }
		inline GLint GetRows(void) const { return m_rows; }
		inline GLint GetCols(void) const { return m_cols; }

	private:
		GLuint m_texture;
		GLuint m_shader;
		GLfloat m_tile_x, m_tile_y;
		GLint m_rows, m_cols;
	};
}
