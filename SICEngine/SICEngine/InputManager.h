/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Aseem Apastamb
 */
 

#pragma once
#include "SDL_stdinc.h"
#include "SICTypes.h"

namespace sic
{

	class InputManager
	{
	public:
		InputManager();
		~InputManager();
		void Update();

		bool isAxisPressedPositive(u8 AxisScanCode);
		bool isAxisPressedNegative(u8 AxisScanCode);
		bool isAxisTriggeredPositive(u8 AxisScanCode);
		bool isAxisTriggeredNegative(u8 AxisScanCode);

		bool isControllerKeyPressed(u8 KeyScanCode);
		bool isControllerKeyReleased(u8 KeyScanCode);
		bool isControllerKeyTriggered(u8 KeyScanCode);

		bool isKeyPressed(unsigned int KeyScanCode);
		bool isKeyTriggered(unsigned int KeyScanCode);
		bool isKeyReleased(unsigned int KeyScanCode);

		bool isMouseButtonPressed(unsigned int KeyScanCode);
		bool isMouseButtonTriggered(unsigned int KeyScanCode);
		bool isMouseButtonReleased(unsigned int KeyScanCode);
		glm::ivec2 GetMousePosition();
		vec3 GetMouseWorldVector();
	public:

	private:

	private:
		i16 mCurrentControllerAxisState[6];
		i16 mPreviousControllerAxisState[6];
		u8 mCurrentControllerButtonState[21];
		u8 mPreviousControllerButtonState[21];
		u8 mCurrentState[512];
		u8 mPreviousState[512];
		u32 mCurrentMouseState;
		u32 mPreviousMouseState;
		int x, y;
	};
}