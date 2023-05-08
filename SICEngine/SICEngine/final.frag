/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: - Andrew Rudasics
 */
 

/////////////////////////////////////////////////////////////////////////
// Fragment shader
////////////////////////////////////////////////////////////////////////
#version 330

in vec2 uvCoord;
in vec2 textCoords;

out vec4 FragColor;

uniform sampler2D tex;

uniform bool InvertColors;
uniform bool IsSplash;
uniform float alpha;

void main()
{
	if (InvertColors){
		FragColor = vec4( vec3(1.0f) - texture(tex, uvCoord).xyz, texture(tex, uvCoord).w);
	}
	else if (IsSplash) {
		FragColor = texture(tex, uvCoord);
		FragColor.w = alpha;
	}
	else {
		FragColor = texture(tex, uvCoord);
	}
	
	if(FragColor.a < 0.1)
		discard;
	
}
