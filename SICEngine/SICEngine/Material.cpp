/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: -
 */
 

#include "Material.h"

namespace sic
{
	Material::Material()
	{
		m_shader = m_texture = 0;
		m_tile_x = m_tile_y = 1.0f;
		m_rows = m_cols = 1;
	}

	Material::Material(GLuint tex, GLuint shader)
	{
		m_texture = tex;
		m_shader = shader;
		m_tile_x = m_tile_y = 1.0f;
		m_rows = m_cols = 1;
	}

	Material::Material(GLuint tex, GLuint shader, float tile_x, float tile_y)
	{
		m_texture = tex;
		m_shader = shader;
		m_tile_x = tile_x;
		m_tile_y = tile_y;
		m_rows = m_cols = 1;
	}

	Material::Material(GLuint tex, GLuint shader, float tile_x, float tile_y, int rows, int cols)
	{
		m_texture = tex;
		m_shader = shader;
		m_tile_x = tile_x;
		m_tile_y = tile_y;
		m_rows = rows;
		m_cols = cols;
	}

	Material::~Material()
	{

	}

	GLuint Material::getShader()
	{
		return m_shader;
	}

	void Material::Use()
	{
		// Use Shader
		glUseProgram(m_shader);

		int loc = 0;

		// Load Texture Uniform
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		loc = glGetUniformLocation(m_shader, "texture");
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(m_shader, "tileX");
		glUniform1f(loc, m_tile_x);

		loc = glGetUniformLocation(m_shader, "tileY");
		glUniform1f(loc, m_tile_y);

		loc = glGetUniformLocation(m_shader, "rows");
		glUniform1i(loc, m_rows);

		loc = glGetUniformLocation(m_shader, "cols");
		glUniform1i(loc, m_cols);
	}

	void Material::Unuse()
	{
		glUseProgram(0);
	}
}