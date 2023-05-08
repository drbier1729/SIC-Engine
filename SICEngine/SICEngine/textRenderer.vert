/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: -
 */
 

/////////////////////////////////////////////////////////////////////////
// Vertex shader for text rendering
////////////////////////////////////////////////////////////////////////
#version 330

in vec4 vertex;

uniform mat4 Camera;
uniform mat4 Projection;
uniform bool isText;

out vec2 textCoords;

void main()
{   
	if(isText) {
		gl_Position = Projection*vec4(vertex.xy, 1.0, 1.0);
		textCoords = vertex.zw;
	}
	else {	
		mat4 scale_matrix;
		scale_matrix = mat4(10.0, 0.0, 0.0, 0.0,
							0.0, 10.0, 0.0, 0.0,
							0.0,  0.0, 1.0, 0.0,
							0.0,  0.0, 0.0, 1.0);
		gl_Position = scale_matrix*vertex;
	}
	
}
