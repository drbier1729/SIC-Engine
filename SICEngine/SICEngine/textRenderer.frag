/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: -
 */
 

/////////////////////////////////////////////////////////////////////////
// Fragment shader for text rendering
////////////////////////////////////////////////////////////////////////
#version 330

in vec2 textCoords;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 color;
uniform bool isText;
uniform float alpha;

void main()
{ 
	if(isText) {
		vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, textCoords).r);
		FragColor = vec4(color, 1.0) * sampled;
	}
	else {
		FragColor = vec4(0.0, 0.0, 0.0, alpha);
	}
}
