/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#version 330

uniform mat4 WorldInverse, ModelTr, NormalTr;
//uniform mat4 WorldView, WorldInverse, WorldProj, ModelTr, NormalTr;
uniform vec3 lightPos;
//uniform mat4 ShadowMatrix;

in vec4 vertex;
in vec3 vertexNormal;
in vec2 vertexTexture;
in vec3 vertexTangent;

out vec3 normalVec, lightVec, eyeVec;
out vec2 texCoord;
out vec4 shadowCoord;
out vec3 tanVec;

void LightingPixel(vec3 eye) {
	vec3 worldPos = (ModelTr*vertex).xyz;

    normalVec = vertexNormal*mat3(NormalTr); 
    lightVec  = lightPos - worldPos;
    eyeVec    = eye - worldPos;

    texCoord    = vertexTexture; 
    //shadowCoord = ShadowMatrix * ModelTr * vertex;
    tanVec      = mat3(ModelTr)*vertexTangent;
}