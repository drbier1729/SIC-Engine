/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Brian Chen
 */
 

/////////////////////////////////////////////////////////////////////////
// Vertex shader
////////////////////////////////////////////////////////////////////////
#version 330

in vec4 vertex;
in vec2 uv;

uniform mat4 ModelTr;
uniform mat4 Camera;
uniform mat4 Projection;
uniform float tileX, tileY;
uniform int rows, cols;
uniform int frame;
uniform bool flip;

out vec2 uvCoord;
out vec2 textCoords;

void main()
{      
    gl_Position = Camera*ModelTr*vertex;

    float cellWidth = 1.0f / cols;
    float cellHeight = 1.0f / rows;
    float posU = 0;
    if (flip)
    {
        posU = ((tileX * (1.0 - uv.x)) * cellWidth) + (frame % cols) * cellWidth;
    }
    else
    {
        posU = ((tileX * uv.x) * cellWidth) + (frame % cols) * cellWidth;
    }    
    
    float posV = ((tileY * (1.0f - uv.y)) * cellHeight) + (frame / cols) * cellHeight;

    uvCoord = vec2(posU, posV);//vec2((tileX * uv.x) * cellWidth + offsetX, (tileY * (1.0f - uv.y)) * cellHeight + offsetY);
}
