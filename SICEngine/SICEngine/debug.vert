/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

/////////////////////////////////////////////////////////////////////////
// Vertex shader for debug drawing colliders
////////////////////////////////////////////////////////////////////////
#version 330

in vec4 vertex;
in vec2 uv;

out vec2 uvCoord;

uniform mat4 Camera;
uniform mat4 ModelTr;

void main()
{      
    gl_Position = Camera*ModelTr*vertex;
    uvCoord = uv;
}