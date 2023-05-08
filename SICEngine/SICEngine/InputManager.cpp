/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Aseem Apastamb
 */
 

/* ------------------------------------------------------------------------
* Copyright(C) 2021 DigiPen Institute of Technology.
* Reproduction or disclosure of this file or its contents without the
* prior written consent of DigiPen Institute of Technology is
* prohibited.
* File Name: InputManager.cpp
* Language: C++ compiled with Visual Studio 2019
* Project: CS 529 Milestone 1
* Author: Andrew Rudasics, andrew.rudasics@digipen.edu
* Creation date: 10/11/21
*
* Purpose: This is the implementation of the interface defined
* in InputManager.h
------------------------------------------------------------------------ */

#include "InputManager.h"
#include "memory.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include <SDL_events.h>
#include "System.h"
#include "Camera.h"

namespace sic
{

	InputManager::InputManager()
	{
		memset(mCurrentState, 0, 512 * sizeof(u8));
		memset(mPreviousState, 0, 512 * sizeof(u8));

		memset(mCurrentControllerAxisState, 0, 6 * sizeof(i16));
		memset(mPreviousControllerAxisState, 0, 6 * sizeof(i16));

		memset(mCurrentControllerButtonState, 0, 15 * sizeof(u8));
		memset(mPreviousControllerButtonState, 0, 15 * sizeof(u8));

		mCurrentMouseState = 0;
		mPreviousMouseState = 0;
	}

	InputManager::~InputManager()
	{

	}

	void InputManager::Update()
	{
		// Keyboard
		memcpy(mPreviousState, mCurrentState, 512 * sizeof(u8));
		int numberOfItems = 0;
		const u8* currentKeyStates = SDL_GetKeyboardState(&numberOfItems);
		if (numberOfItems > 512)
			numberOfItems = 512;
		memcpy(mCurrentState, currentKeyStates, numberOfItems * sizeof(u8));

		// Controller
		// Axes
		memcpy(mPreviousControllerAxisState, mCurrentControllerAxisState, 6 * sizeof(i16));
		numberOfItems = 6;
		for (u8 i = 0; i < 6; ++i) {
			mCurrentControllerAxisState[i] = SDL_JoystickGetAxis(System::Controller(), i);
		}

		// Buttons
		memcpy(mPreviousControllerButtonState, mCurrentControllerButtonState, 21 * sizeof(u8));
		numberOfItems = 21;
		for (u8 i = 0; i < 21; ++i) {
			mCurrentControllerButtonState[i] = SDL_JoystickGetButton(System::Controller(), i);
		}

		// Mouse
		mPreviousMouseState = mCurrentMouseState;
		mCurrentMouseState = SDL_GetMouseState(&x, &y);
	}

	bool InputManager::isAxisPressedPositive(u8 AxisScanCode) {
		if (AxisScanCode >= 6) {
			return false;
		}
		return (mCurrentControllerAxisState[AxisScanCode] > System::Settings().controller_dead_zone);
	}

	bool InputManager::isAxisPressedNegative(u8 AxisScanCode) {
		if (AxisScanCode >= 6) {
			return false;
		}
		return (mCurrentControllerAxisState[AxisScanCode] < -System::Settings().controller_dead_zone);
	}

	bool InputManager::isAxisTriggeredPositive(u8 AxisScanCode) {
		if (AxisScanCode >= 6) {
			return false;
		}
		return (mCurrentControllerAxisState[AxisScanCode] > System::Settings().controller_dead_zone) &&
			   (mPreviousControllerAxisState[AxisScanCode] < System::Settings().controller_dead_zone);
	}

	bool InputManager::isAxisTriggeredNegative(u8 AxisScanCode) {
		if (AxisScanCode >= 6) {
			return false;
		}
		return (mCurrentControllerAxisState[AxisScanCode] < -System::Settings().controller_dead_zone) &&
			   (mPreviousControllerAxisState[AxisScanCode] > -System::Settings().controller_dead_zone);
	}

	bool InputManager::isControllerKeyPressed(u8 KeyScanCode)
	{
		if (KeyScanCode >= 21)
			return false;

		return mCurrentControllerButtonState[KeyScanCode];
	}

	bool InputManager::isControllerKeyReleased(u8 KeyScanCode)
	{
		if (KeyScanCode >= 21)
			return false;

		return (!mCurrentControllerButtonState[KeyScanCode] && mPreviousControllerButtonState[KeyScanCode]);
	}

	bool InputManager::isControllerKeyTriggered(u8 KeyScanCode)
	{
		if (KeyScanCode >= 21)
			return false;

		return (mCurrentControllerButtonState[KeyScanCode] && !mPreviousControllerButtonState[KeyScanCode]);
	}

	bool InputManager::isKeyPressed(unsigned int KeyScanCode)
	{
		if (KeyScanCode >= 512)
			return false;

		return mCurrentState[KeyScanCode];
	}

	bool InputManager::isKeyReleased(unsigned int KeyScanCode)
	{
		if (KeyScanCode >= 512)
			return false;

		return (!mCurrentState[KeyScanCode] && mPreviousState[KeyScanCode]);
	}

	bool InputManager::isKeyTriggered(unsigned int KeyScanCode)
	{
		if (KeyScanCode >= 512)
			return false;

		return (mCurrentState[KeyScanCode] && !mPreviousState[KeyScanCode]);
	}

	bool InputManager::isMouseButtonPressed(unsigned int KeyScanCode)
	{
		return KeyScanCode & mCurrentMouseState;
	}

	bool InputManager::isMouseButtonReleased(unsigned int KeyScanCode)
	{
		return !(KeyScanCode & mCurrentMouseState) && (KeyScanCode & mPreviousMouseState);
	}

	bool InputManager::isMouseButtonTriggered(unsigned int KeyScanCode)
	{
		return static_cast<bool>(KeyScanCode & mCurrentMouseState) && !static_cast<bool>(KeyScanCode & mPreviousMouseState);
	}

	glm::ivec2 InputManager::GetMousePosition()
	{
		return glm::ivec2(x, y);
	}

	vec3 InputManager::GetMouseWorldVector()
	{
		mat4 matProj = System::Cam().GetViewMatrix() * System::Cam().GetProjectionMatrix();
		mat4 matInv = glm::inverse(matProj);

		vec2 mouseScreen = vec2{ static_cast<f32>(x), static_cast<f32>(y) };

		f32 in[4];
		f32 winZ = 1.0f;
		in[0] = 1.0f - (2.0f * mouseScreen.x / System::Settings().screen_width);
		in[1] = (2.0f * mouseScreen.y / System::Settings().screen_height) - 1.0f;
		in[2] = 2.0f * winZ - 1.0f; // TODO : check if z is correct
		in[3] = 1.0f;

		vec4 vIn = vec4(in[0], in[1], in[2], in[3]);
		vec4 worldPos = vIn * matInv;

		worldPos.x /= worldPos.w;
		worldPos.y /= worldPos.w;
		worldPos.z /= worldPos.w;

		//worldPos.x += System::Cam().GetPosition().x;
		//worldPos.y += System::Cam().GetPosition().y;

		return worldPos.xyz;
	}
}