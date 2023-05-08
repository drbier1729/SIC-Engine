/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier
 */
 

#include "ImGuiWindow.h"

#ifdef _DEBUG
#include <cstdio>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_sdl.h"
#include "externals/imgui/imgui_impl_opengl3.h"

#include <SDL_video.h>

#include "System.h"
#include "FrameRateController.h"
#include "MemoryManager.h"
#include "MemResources.h"
#include "Camera.h"
#include "Components.h"
#include "ComponentSystems.h"
#include "Entities.h"
#include "EventManager.h"
#include "StringId.h"
#include "Reflector.h"
#include "Factory.h"
#include "EventManager.h"

#include "externals/World.h"
#endif

#ifdef _DEBUG

namespace sic {

	// template helpers
	template<ValidComponent C>
	void MakeComponentWidget(C& comp);

	template<std::integral I>
	void DragScalarInt(char const* label, ImGuiDataType dataType, I* value, I min = 0, I max = 0, char const* format = "%lld");

	template<std::floating_point F>
	void DragScalarFloat(char const* label, ImGuiDataType dataType, F* value, F min = 0.0f, F max = 0.0f, char const* format = "%lf");
}


namespace sic {


	void ImGuiWindow::Create()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForOpenGL(System::Window(), System::GLContext());
		ImGui_ImplOpenGL3_Init("#version 330"); // TODO (dylan) : string should be read in from System::Settings()
	}

	void ImGuiWindow::Destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiWindow::HandleSDLEvent(SDL_Event& e)
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
	}

	void ImGuiWindow::BeginRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(System::Window());
		ImGui::NewFrame();
	}

	void ImGuiWindow::EndRender()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(System::Window(), System::GLContext());
		}
	}

	void ImGuiWindow::Tools() {
#ifdef _DEBUG
		if (System::Settings().mem_dbg_mode) {
			FrameRateTool();
			MemoryTool();
			GraphicsTool();
		}
#endif
	}

	void ImGuiWindow::FrameRateTool() {
		f32 dt = System::FrameRate().DeltaTime();
		f32 fr = System::FrameRate().FrameRate();
		f32 fr_clamped = fr / 60.0f;
		char buf[32];
		sprintf_s(buf, "%.1f/%.1f", fr, 60.0f);

		ImGui::Begin("Frame Rate Tool");
		ImGui::Text("Delta Time: %.4f sec/frame\n", dt);
		ImGui::Text("Frame Rate: %.1f FPS\n", fr);
		ImGui::ProgressBar(fr_clamped, ImVec2(0.0f, 0.0f), buf);
		ImGui::End();
	}

	void ImGuiWindow::MemoryTool() {
		assert(System::Settings().mem_dbg_mode && "Not in debug mode!");
		
		u64 dyn_used = static_cast<DebugMemResource*>(System::Memory().Dynamic())->DynamicUsed();
		u64 dyn = System::Settings().dynamic_memory_size;
		f32 dyn_used_clamped = static_cast<f32>(dyn_used) / dyn;
		char buf1[32];
		sprintf_s(buf1, "%llu/%llu", dyn_used, dyn);
		u64 frm_used = static_cast<DebugMemResource*>(System::Memory().Frame())->FrameUsed();
		u64 frm = System::Settings().frame_memory_size;
		f32 frm_used_clamped = static_cast<f32>(frm_used) / frm;
		char buf2[32];
		sprintf_s(buf2, "%llu/%llu", frm_used, frm);
		u64 per_used = static_cast<DebugMemResource*>(System::Memory().Persistent())->PersistentUsed();
		u64 per = System::Settings().persistent_memory_size;
		f32 per_used_clamped = static_cast<f32>(per_used) / per;
		char buf3[32];
		sprintf_s(buf3, "%llu/%llu", per_used, per);

		ImGui::Begin("Memory Tool");
		ImGui::Text("Dynamic Memory: %llu\n", dyn_used);
		ImGui::ProgressBar(dyn_used_clamped, ImVec2(0.0f, 0.0f), buf1);
		ImGui::Text("Frame Memory: %llu\n", frm_used);
		ImGui::ProgressBar(frm_used_clamped, ImVec2(0.0f, 0.0f), buf2);
		ImGui::Text("Persistent Memory: %llu\n", per_used);
		ImGui::ProgressBar(per_used_clamped, ImVec2(0.0f, 0.0f), buf3);
		ImGui::End();
	}

	void ImGuiWindow::GraphicsTool() {
		assert(System::Settings().mem_dbg_mode && "Not in debug mode!");

		Camera* cam = &System::Cam();
		vec3 pos = cam->GetPosition();

		ImGui::Begin("Graphics Tool");
		ImGui::Text("position: %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);
		if (ImGui::Button(cam->Debug() ? "Follow Cam" : "Free Cam")) { cam->ToggleDebugCam(); }
		if (ImGui::Button(cam->DebugCollider() ? "Show Collider: On" : "Show Collider: Off")) { cam->ToggleCollider(); }
		//ImGui::Text(strCout.str().c_str());
		ImGui::End();
	}

#ifdef _DEBUG
	void ImGuiWindow::SceneEditor()
	{
		// ----------------
		// Entity List
		// ----------------
		ImGui::Begin("Scene View");
		local_persistent EntityID selected{ NullEntityId };
		EntityList(selected);
		ImGui::End();

		// ----------------
		// Entity Inspector
		// ----------------
		if (System::ECS().IsValidEntity(selected))
		{
			EntityInspector(selected);
		}
	}
#else
	void ImGuiWindow::SceneEditor() { }
#endif

#ifdef _DEBUG
	void ImGuiWindow::EntityList(EntityID& selected) {
		if (ImGui::BeginTable("Entity List", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("EntityID");
			ImGui::TableSetupColumn("Name");
			ImGui::TableHeadersRow();
			System::ECS().ForEachEntity([&](Entity const& e) {

				char entity_label[20];
				sprintf_s(entity_label, "%llu", e.Id());
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				if (ImGui::Selectable(entity_label, selected == e.Id(), ImGuiSelectableFlags_SpanAllColumns))
				{
					selected = e.Id();
				}
				ImGui::TableNextColumn();
				if (e.Has<Name>())
				{
					auto name_id = e.Get<Name>().name.Id();
					const char* name = System::Dictionary().StrLookUp(name_id);
					ImGui::Text(name);
				}
				else
				{
					ImGui::Text("--");
				}

			});
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::Button("New Entity"))
			{
				System::ECS().MakeEntity();
			}
			// ----------------
			// New Entity From Archetype...
			// ----------------
			NewEntityFromArchetype();
			ImGui::EndTable();

			ImGui::Spacing();

			// ----------------
			// Save Scene As...
			// ----------------
			SaveSceneAs();

			// ----------------
			// Load Scene...
			// ----------------
			LoadScene();

			// ----------------
			// Reload Scene...
			// ----------------
			ReloadScene();
		}
	}

	void ImGuiWindow::NewEntityFromArchetype() {
		char buf[100] = "";
		local_persistent char result[128];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("New Entity From Archetype...", buf, 100, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			if (System::Resources().ValidFilepath(hash))
			{
				System::SceneFactory().BuildEntityFromArchetype(hash);

				sprintf_s(result, "Created from: %s", buf);
			}
			else
			{
				sprintf_s(result, "File not found: %s", buf);
			}
		}
	}

	void ImGuiWindow::SaveSceneAs() {
		char buf[100] = "";
		local_persistent char result[128];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("Save Scene As...", buf, 100, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			if (System::Resources().ValidFilepath(hash))
			{
				System::SceneFactory().SaveScenePretty(hash);
				System::Resources().WriteJsonToDisk(hash);

				sprintf_s(result, "Saved to: %s", buf);
			}
			else
			{
				sprintf_s(result, "File not found: %s", buf);
			}
		}
		ImGui::Text("%s", result);
	}

	void ImGuiWindow::LoadScene() {
		char buf[100] = "";
		local_persistent char result[128];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("Load From...", buf, 100, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			if (System::Resources().ReloadJsonFromDisk(hash))
			{
				System::Events().Reset();
				System::Physics().Clear();
				System::ECS().Reset();

				System::SceneFactory().BuildScene(hash);

				System::ECS().ForEach<Body>(WriteDataToPhysicsWorldSystem);
				if (!System::Cam().Debug()) { System::ECS().ForEach<PlayerController>(FollowCamSystem); }

				System::Events().Reset();

				currentSceneSID = hash;

				sprintf_s(result, "Loaded From: %s", buf);
			}
			else
			{
				sprintf_s(result, "File not found: %s", buf);
			}
		}
		ImGui::Text("%s", result);
	}

	void ImGuiWindow::ReloadScene()
	{
		if (ImGui::Button("Reload Last Loaded"))
		{
			if (System::Resources().ReloadJsonFromDisk(currentSceneSID))
			{
				System::Events().Reset();
				System::Physics().Clear();
				System::ECS().Reset();

				System::SceneFactory().BuildScene(currentSceneSID);

				System::ECS().ForEach<Body>(WriteDataToPhysicsWorldSystem);
				if (!System::Cam().Debug()) { System::ECS().ForEach<PlayerController>(FollowCamSystem); }

				System::Events().Reset();
			}
		}
	}

	void ImGuiWindow::EntityInspector(EntityID& selected) {
		ImGui::Begin("Entity Inspector");
		if (ImGui::BeginTable("##ComponentTypes", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Component");
			local_persistent ComponentID item_current_idx = NUM_COMPONENTS;

			Entity entity = System::ECS().GetEntityFromID(selected);
			ComponentID marked_for_removal{ NUM_COMPONENTS };

			entity.ForEachComponent([&marked_for_removal](ValidComponent auto& c) {
				const bool is_selected = (item_current_idx == c.id);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				if (ImGui::Selectable(ComponentNameFromId(c.id), is_selected, ImGuiSelectableFlags_SpanAllColumns))
				{
					item_current_idx = c.id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					ImGui::Begin("Component Editor");
					MakeComponentWidget(c);
					ImGui::End();
				}
			});
			ImGui::EndTable();

			ImGui::Spacing();

			// -----------------------
			// Add Component...
			// -----------------------
			AddComponent(selected);

			ImGui::Spacing();

			// -----------------------
			// Save Entity To Scene...
			// -----------------------
			SaveEntityToScene(entity);

			// -----------------------
			// Save Entity as Archetype...
			// -----------------------
			SaveEntityAsArchetype(entity);
						
			if (ImGui::Button("Destroy Entity"))
			{
				if (entity.Has<Body>()) {
					System::Physics().Remove(entity);
				}
				System::ECS().DestroyEntity(entity);
			}
		}
		ImGui::End();
	}

	void ImGuiWindow::AddComponent(EntityID& selected) {
		char buf[32] = "";
		local_persistent char result[64];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("Add Component...", buf, 32, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			ComponentType type = ComponentTypeFromName(hash);
			ComponentID cid = static_cast<ComponentID>(type);
			if (type == ComponentType::INVALID)
			{
				sprintf_s(result, "%s is not a valid component", buf);
			}
			else if (System::ECS().HasComponent(cid, selected))
			{
				sprintf_s(result, "Entity already has a %s", buf);
			}
			else
			{
				void* comp = System::ECS().AddComponent(selected, cid);
				if (type == ComponentType::Body) {
					Entity e = System::ECS().GetEntityFromID(selected);
					System::Physics().Add(e);
				}
				sprintf_s(result, "");
			}
		}
		ImGui::Text("%s", result);
	}

	void ImGuiWindow::SaveEntityToScene(Entity& entity) {
		char buf[100] = "";
		local_persistent char result[128];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("Save Entity to Scene...", buf, 100, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			if (System::Resources().ValidFilepath(hash))
			{
				System::SceneFactory().SaveEntityInstanceInScene(hash, entity);

				// TODO (dylan) : do we want this here? Or should we just write to disk during TestGame shutdown?
				System::Resources().WriteJsonToDisk(hash);

				sprintf_s(result, "Saved Entity to Scene: %s", buf);
			}
			else
			{
				sprintf_s(result, "Scene file not found: %s", buf);
			}
		}
		ImGui::Text("%s", result);
	}

	void ImGuiWindow::SaveEntityAsArchetype(Entity& entity) {
		char buf[100] = "";
		local_persistent char result[128];
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("Save Entity as Archetype...", buf, 100, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			auto hash = StrHash(buf);
			if (System::Resources().ValidFilepath(hash))
			{
				System::SceneFactory().SaveEntityAsArchetype(hash, entity);

				// TODO (dylan) : do we want this here? Or should we just write to disk during TestGame shutdown?
				System::Resources().WriteJsonToDisk(hash);

				sprintf_s(result, "Saved Entity As: %s", buf);
			}
			else
			{
				sprintf_s(result, "Archetype not found: %s", buf);
			}
		}
		ImGui::Text("%s", result);
	}
#endif

}

namespace sic {

	template<ValidComponent C>
	void MakeComponentWidget(C& comp)
	{
		Reflector::Class const* objectInfo = Reflector::GetClass<C>();

		for (auto const& field : objectInfo->fields) {
			if (field.type == nullptr) {
				break;
			}

			void* source = reinterpret_cast<i8*>(&comp) + field.offset;
			switch (field.type->enumName) {
				case Reflector::TypeName::u8: {
					u8* src = static_cast<u8*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<u8>(label, ImGuiDataType_U8, src, 0, UINT8_MAX, "%hhu");
					break;
				}
				case Reflector::TypeName::u16: {
					u16* src = static_cast<u16*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<u16>(label, ImGuiDataType_U16, src, 0, UINT16_MAX, "%hu");
					break;
				}
				case Reflector::TypeName::u32: {
					u32* src = static_cast<u32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<u32>(label, ImGuiDataType_U32, src, 0, UINT32_MAX, "%lu");
					break;
				}
				case Reflector::TypeName::u64: {
					u64* src = static_cast<u64*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<u64>(label, ImGuiDataType_U64, src, 0, UINT64_MAX, "%llu");
					break;
				}
				case Reflector::TypeName::i8: {
					i8* src = static_cast<i8*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<i8>(label, ImGuiDataType_S8, src, INT8_MIN, INT8_MAX, "%hhd");
					break;
				}
				case Reflector::TypeName::i16: {
					i16* src = static_cast<i16*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<i16>(label, ImGuiDataType_S16, src, INT16_MIN, INT16_MAX, "%hd");
					break;
				}
				case Reflector::TypeName::i32: {
					i32* src = static_cast<i32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<i32>(label, ImGuiDataType_S32, src, INT32_MIN, INT32_MAX, "%ld");
					break;
				}
				case Reflector::TypeName::i64: {
					i64* src = static_cast<i64*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarInt<i64>(label, ImGuiDataType_S64, src, INT64_MIN, INT64_MAX, "%lld");
					break;
				}
				case Reflector::TypeName::f32: {
					f32* src = static_cast<f32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarFloat<f32>(label, ImGuiDataType_Float, src, -FLT_MAX, FLT_MAX, "%.3f");
					break;
				}
				case Reflector::TypeName::f64: {
					f64* src = static_cast<f64*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					DragScalarFloat<f64>(label, ImGuiDataType_Double, src, -FLT_MAX, DBL_MAX, "%.3lf");
					break;
				}
				case Reflector::TypeName::vec2: {
					f32* src = static_cast<f32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					ImGui::DragFloat2(label, src, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
					break;
				}
				case Reflector::TypeName::vec3: {
					f32* src = static_cast<f32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					ImGui::DragFloat3(label, src, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
					break;
				}
				case Reflector::TypeName::vec4: {
					f32* src = static_cast<f32*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					ImGui::DragFloat4(label, src, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
					break;
				}
				case Reflector::TypeName::StringId: {
					StringId* src = static_cast<StringId*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					char buf[128] = "";
					ImGui::SetNextItemWidth(200.0f);
					ImGui::Text("%s: %s", label, src->Str());
					ImGui::SetNextItemWidth(200.0f);
					if (ImGui::InputText("##TextBox", buf, 128, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						*src = StringId{ buf };
					}
					break;
				}
				case Reflector::TypeName::bool8: {
					bool8* src = static_cast<bool8*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					ImGui::Checkbox(label, src);
					break;
				}
				case Reflector::TypeName::EntityID: {
					EntityID* src = static_cast<EntityID*>(source);
					char label[32];
					sprintf_s(label, " %s [%s]", field.name, field.type->stringName);
					ImGui::Text("%s: %llu", label, *src);
					break;
				}
				default: {
					break;
				}
			}
		}
		if (ImGui::Button("Remove Component"))
		{
			Entity owner = System::ECS().GetOwner(comp);
			if constexpr (C::id == Body::id) {
				System::Physics().Remove(&comp);
			}
			owner.Remove<C>();
		}
	}

	template<std::integral I>
	void DragScalarInt(char const* label, ImGuiDataType dataType, I* value, I min, I max, char const* format) {
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragScalar(label, dataType, value, 0.1f, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
	}

	template<std::floating_point F>
	void DragScalarFloat(char const* label, ImGuiDataType dataType, F* value, F min, F max, char const* format) {
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragScalar(label, dataType, value, 0.1f, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
	}

}

#else
namespace sic {
	void ImGuiWindow::Create() {}
	void ImGuiWindow::Destroy() {}
	void ImGuiWindow::HandleSDLEvent(SDL_Event& e) {}
	void ImGuiWindow::BeginRender() {}
	void ImGuiWindow::EndRender() {}
	void ImGuiWindow::Tools() {}
	void ImGuiWindow::SceneEditor() {}
	void ImGuiWindow::FrameRateTool() {}
	void ImGuiWindow::MemoryTool() {}
	void ImGuiWindow::GraphicsTool() {}
	void ImGuiWindow::EntityList(EntityID& selected) {}
	void ImGuiWindow::NewEntityFromArchetype() {}
	void ImGuiWindow::SaveSceneAs() {}
	void ImGuiWindow::LoadScene() {}
	void ImGuiWindow::ReloadScene() {}
	void ImGuiWindow::EntityInspector(EntityID& selected) {}
	void ImGuiWindow::AddComponent(EntityID& selected) {}
	void ImGuiWindow::SaveEntityToScene(Entity& entity) {}
	void ImGuiWindow::SaveEntityAsArchetype(Entity& entity) {}
}
#endif