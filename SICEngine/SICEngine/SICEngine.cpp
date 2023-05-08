/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

// Windows
#ifdef _DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif 
#endif

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

// STL
#include <iostream>
#include <cassert>

// Other Dependencies
#include <glew.h>
#include <SDL.h>

// SIC Engine Dependencies
#include "SICTypes.h"
#include "System.h"
#include "MemoryManager.h"

#include "TestGame.h"
#include "ActionPlatformer.h"
#include "StealthLevel.h"
#include "PuzzlePrototype.h"

// TODO: Random junk or valuable code ???
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#pragma comment(lib, "legacy_stdio_definitions.lib")


int main(int argc, char* args[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// System Init
	sic::SystemOptions sys_opts = sic::ConfigSystem("EngineConfig.json");

	// Initialize engine
	if (!sic::System::Init(sys_opts))
	{
		std::cout << "\nSystem failed to initialize" << '\n';
		return 1;
	}

	
	// Create game instance
	auto& memory_manager{ sic::System::Memory() };
	auto  persistent_memory{ memory_manager.Persistent() };

	sic::Application* game = nullptr;
	char gm;
	if (argc > 1)
	{
		gm = *args[1];

		if (gm == '0')
		{
			game = memory_manager.Create<TestGame::TestGame>(persistent_memory);
		}
		else if (gm == '1')
		{
			game = memory_manager.Create<PuzzlePrototype::PuzzlePrototype>(persistent_memory);
		}
	}
	else
	{
		game = memory_manager.Create<PuzzlePrototype::PuzzlePrototype>(persistent_memory);
	}
	
	assert(game != nullptr);

	// Run game
	sic::System::Run(game);

	// Destroy game instance
	memory_manager.Destroy(persistent_memory, game);

	// Shutdown
	sic::System::Close();
	

	return 0;
}