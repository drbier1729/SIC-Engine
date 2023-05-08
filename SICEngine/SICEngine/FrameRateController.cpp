/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: -
 */
 

#include "FrameRateController.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <chrono>

namespace sic{

	using namespace std::chrono;

	FrameRateController::FrameRateController(u32 maxFrameRate):
		m_start_ticks{ 0 },
		m_end_ticks{ 0 },
		m_max_frame_rate{ maxFrameRate },
		m_max_ticks_per_frame{ 1000000 / m_max_frame_rate },
		m_delta_time{ 0 },
		m_frame_counter{ 0 },
		m_frame_rate_timer{ 0 },
		m_frame_rate{ 0 }
	{
		assert(m_max_frame_rate > 0 && "Maximum Frame Rate cannot be 0 or less.");
	}

	void FrameRateController::FrameStart() {
		auto now = time_point_cast<microseconds>(steady_clock::now());
		m_start_ticks = now.time_since_epoch().count();
	}

	void FrameRateController::FrameEnd() {
		auto now = time_point_cast<microseconds>(steady_clock::now());
		m_end_ticks = now.time_since_epoch().count();
		i64 elapsedTime = m_end_ticks - m_start_ticks;
		while (elapsedTime < m_max_ticks_per_frame) {
			now = time_point_cast<microseconds>(steady_clock::now());
			m_end_ticks = now.time_since_epoch().count();
			elapsedTime = m_end_ticks - m_start_ticks;
		}
		m_delta_time = static_cast<f32>(elapsedTime) / 1000000.0f;

		// calculating frame rate over 120 frames
		++m_frame_counter;
		m_frame_rate_timer += elapsedTime;
		if (m_frame_counter == 120) {
			//u32 bar_width{ m_max_frame_rate };
			m_frame_rate = 120000000 / static_cast<f32>(m_frame_rate_timer);
			//std::cout << std::setw(26) << "Frame Rate Avg (over 120 frames): " << std::setprecision(3) << m_frame_rate << std::endl;

		//	// Bar visualization
		//	u32 avgFRint{ static_cast<u32>(avgFR) };
		//	std::cout << "(0 FPS) [";
		//	for (u32 i = 0; i < avgFRint; ++i) { std::cout << "#"; }
		//	for (u32 j = avgFRint; j < bar_width; ++j) { std::cout << "."; };
		//	std::cout << "] (" << bar_width << " FPS)" << std::endl;

			m_frame_counter = 0;
			m_frame_rate_timer = 0;

			//std::cout << "Delta Time: " << m_delta_time << std::endl;
		}
	}

	f32 FrameRateController::DeltaTime() {
		return m_delta_time;
	}

	f32 FrameRateController::FrameRate() {
		return m_frame_rate;
	}

}