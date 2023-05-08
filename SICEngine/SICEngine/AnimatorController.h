/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: -
 */
 
// Not in use
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "StringId.h"

namespace sic
{
	struct AnimClip
	{
		AnimClip() : state(0), start_idx(0), end_idx(0) {}

		AnimClip(u32 st, i32 start, i32 end) : state(st), start_idx(start), end_idx(end) { }

		u32 state;
		i32 start_idx;
		i32 end_idx;
		

	};

	// TODO : unused. can we delete?
	class AnimatorController
	{
	public:
		void SetState(std::string state);
		void Update();
		unsigned int GetFrame();
		

	private:
		unsigned int rows, cols;
		std::string current_clip;
		unsigned int current_frame;
		std::pmr::vector<unsigned int>* current_clip_array;
		int frame_counter;

		std::pmr::unordered_map<sic::StringId::underlying_type, std::pmr::vector<unsigned int>*> clips;
	};

}
