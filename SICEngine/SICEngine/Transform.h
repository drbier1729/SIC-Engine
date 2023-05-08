/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: -
 */
 

#pragma once
#include <fstream>
#include <glm.hpp>

namespace sic
{
	// Factory functions to create specific transformations, multiply two, and invert one.
	glm::mat4 Rotate(const int i, const float theta);
	glm::mat4 Scale(const float x, const float y, const float z);
	glm::mat4 Scale(const glm::vec3& s);
	glm::mat4 Translate(const float x, const float y, const float z);
	glm::mat4 Translate(const glm::vec3 &pos);
	glm::mat4 Perspective(const float rx, const float ry,
		const float front, const float back);
	glm::mat4 LookAt(const glm::vec3 Eye, const glm::vec3 Center, const glm::vec3 Up);

	float* Pntr(glm::mat4& m);
}