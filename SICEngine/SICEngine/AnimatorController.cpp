/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: -
 */
 

#include "AnimatorController.h"

namespace sic
{
	void AnimatorController::Update()
	{
		/*if (frame_counter < 2)
		{
			frame_counter++;
		}
		else
		{
			frame_counter = 0;

			std::pmr::vector<unsigned int>* clip_array = clips[current_clip];

			if (clip_array != nullptr)
			{
				if (current_frame == clip_array->size() - 1)
				{
					current_frame = 0;
				}
				else
				{
					current_frame++;
				}
			}
		}*/
	}

	void AnimatorController::SetState(std::string clip_name)
	{
		/*current_clip = clip_name;
		current_clip_array = clips[current_clip];
		current_frame = 0;*/
	}

	unsigned int AnimatorController::GetFrame()
	{
		return current_frame;
	}
}