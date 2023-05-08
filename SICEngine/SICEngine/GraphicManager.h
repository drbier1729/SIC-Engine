/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#pragma once
#include "StringId.h"
#include "SICTypes.h"
#include <map>
//#include <ft2build.h>
//#include FT_FREETYPE_H

namespace sic
{
    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    class GraphicManager
    {
    public:
        GraphicManager();

        ~GraphicManager();

        void Init();

        std::map<char, Character>& GetCharacters() { return Characters; }

    private:
        std::map<char, Character> Characters;
        
    };

}