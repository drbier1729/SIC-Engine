/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: -
 */
 

#pragma once

#include "SICTypes.h"

namespace sic {

	class FrameRateController
	{
	public:
		FrameRateController() = default;
		FrameRateController(u32 maxFrameRate);
		FrameRateController(FrameRateController const&) = delete;
		FrameRateController& operator=(FrameRateController const&) = delete;
		FrameRateController(FrameRateController&&) = delete;
		FrameRateController& operator=(FrameRateController&&) = delete;
		~FrameRateController() = default;

		void FrameStart();
		void FrameEnd();
		f32 DeltaTime();
		f32 FrameRate();

	private:
		i64 m_start_ticks;
		i64 m_end_ticks;
		u32 m_max_frame_rate;
		i64 m_max_ticks_per_frame;
		f32 m_delta_time;
		u32 m_frame_counter;
		i64 m_frame_rate_timer;
		f32 m_frame_rate;
	};

}