/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "GraphicManager.h"

#include <glew.h>

namespace sic
{
    GraphicManager::GraphicManager()
    {

    }

    GraphicManager::~GraphicManager()
    {

    }

    void GraphicManager::Init()
    {
		/*FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			assert("ERROR::FREETYPE: Could not init FreeType Library");
		}

		FT_Face face;
		if (FT_New_Face(ft, "Assets//Fonts//arial.ttf", 0, &face))
		{
			assert("ERROR::FREETYPE: Failed to load font");
		}

		FT_Set_Pixel_Sizes(face, 0, 48);*/



		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		//for (unsigned char c = 0; c < 128; c++)
		//{
		//	// load character glyph 
		//	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		//	{
		//		assert("ERROR::FREETYTPE: Failed to load Glyph");
		//		continue;
		//	}
		//	// generate texture
		//	unsigned int texture;
		//	glGenTextures(1, &texture);
		//	glBindTexture(GL_TEXTURE_2D, texture);
		//	glTexImage2D(
		//		GL_TEXTURE_2D,
		//		0,
		//		GL_RED,
		//		face->glyph->bitmap.width,
		//		face->glyph->bitmap.rows,
		//		0,
		//		GL_RED,
		//		GL_UNSIGNED_BYTE,
		//		face->glyph->bitmap.buffer
		//	);
		//	// set texture options
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	// now store character for later use
		//	Character character = {
		//		texture,
		//		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		//		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		//		face->glyph->advance.x
		//	};
		//	Characters.insert(std::pair<char, Character>(c, character));
		//}

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//FT_Done_Face(face);
		//FT_Done_FreeType(ft);
    }
}