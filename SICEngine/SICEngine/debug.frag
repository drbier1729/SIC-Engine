/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

/////////////////////////////////////////////////////////////////////////
// Pixel shader for debug drawing colliders
////////////////////////////////////////////////////////////////////////
#version 330

in vec2 uvCoord;

out vec4 FragColor;
uniform sampler2D tex;
uniform vec4 DebugColor;

void main()
{
	if(texture(tex, uvCoord).a < 0.1) { discard; }
	else							  { FragColor = DebugColor; }
}
