/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Aseem Apastamb
 */
 

#pragma once

#include "SICTypes.h"

typedef union SDL_Event SDL_Event;

namespace sic {

	struct Entity;

	class ImGuiWindow
	{
	public:
		void Create();
		void Destroy();

		void HandleSDLEvent(SDL_Event& e);

		void BeginRender();
		void EndRender();

		void Tools();
		void SceneEditor();

		inline
		void SetCurrentScene(str_hash_type sid) { currentSceneSID = sid; }

	private:
		void FrameRateTool();
		void MemoryTool();
		void GraphicsTool();

		void EntityList(EntityID& selected);
		void NewEntityFromArchetype();
		void SaveSceneAs();
		void LoadScene();
		void ReloadScene();

		void EntityInspector(EntityID& selected);
		void AddComponent(EntityID& selected);
		void SaveEntityToScene(Entity& entity);
		void SaveEntityAsArchetype(Entity& entity);

		sic::str_hash_type currentSceneSID;
	};
}

