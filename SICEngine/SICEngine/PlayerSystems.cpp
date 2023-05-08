/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Andrew Rudasics, Dylan Bier, Brian Chen
 */
 

#include "ComponentSystems.h"

#include "SICEngine.h"
#include <SDL_scancode.h> // scancodes for keyboard input
#include <SDL_mouse.h> // scancodes for mouse input
#include <SDL_gamecontroller.h> // scancodes for controller input

namespace sic {

	void PlayerControllerSystem(PlayerController& controller, f32 dt) {
		Entity e = System::ECS().GetOwner(controller);

		assert(e.Has<Body>() && "All entities with a controller must have a Body.");
		auto& body = e.Get<Body>();

		controller.elapsed_time += dt;
		vec2 move = vec2{ 0.0f };

		// Sprint
		if (System::Input().isKeyPressed(SDL_SCANCODE_LSHIFT) || System::Input().isControllerKeyPressed(SDL_CONTROLLER_BUTTON_X))
		{
			controller.is_dashing = true;
		}
		else
		{
			controller.is_dashing = false;
		}

		// Move
		if (System::Input().isKeyPressed(SDL_SCANCODE_A) || System::Input().isAxisPressedNegative(SDL_CONTROLLER_AXIS_LEFTX))
		{
			if (controller.is_dashing == true)
			{
				move.x -= 2.0f;
			}
			else
			{
				move.x -= 1.0f;
			}
		}
		if (System::Input().isKeyPressed(SDL_SCANCODE_D) || System::Input().isAxisPressedPositive(SDL_CONTROLLER_AXIS_LEFTX))
		{
			if (controller.is_dashing == true)
			{
				move.x += 2.0f;
			}
			else
			{
				move.x += 1.0f;
			}
		}
		
		//// Fire
		//if (System::Input().isMouseButtonTriggered(SDL_BUTTON_LMASK))
		//{
		//	vec2 mouse = vec2{ 0.0f };
		//	mouse.x = System::Input().GetMouseWorldPosition().x;
		//	mouse.y = System::Input().GetMouseWorldPosition().y;

		//	System::Events().EnqueueEvent(EventType::Shoot, 0.0f, {
		//		{"EntityID_player", e}
		//	});
		//}

		// Jump
		controller.g = (2 * controller.max_jump_height) / (controller.time_to_apex * controller.time_to_apex);
		controller.init_jump_v = glm::sqrt(2 * controller.g * controller.max_jump_height);

		if (body.isGrounded && controller.jump_released) {
			controller.last_grounded_time = controller.elapsed_time;
			controller.can_jump = true;
		}
		if (controller.elapsed_time - controller.last_grounded_time > 0.15f) {
			controller.can_jump = false;
		}

		bool8 jump_input_down = System::Input().isKeyPressed(SDL_SCANCODE_SPACE) || System::Input().isControllerKeyPressed(SDL_CONTROLLER_BUTTON_A);

		if (!controller.jump_released && !jump_input_down)
		{
			controller.jump_released = true;
		}

		if (controller.can_jump) {
			if (jump_input_down) {
				move.y = controller.init_jump_v;
				//auto sound_clip = System::Resources().LoadAudioFile(System::Audio().SoundSystem(), ".\\Assets\\Audio\\boing.mp3");
				//System::Audio().PlayOnce(sound_clip, AudioType::EFFECT);
				if (controller.jump_released) {
					System::Audio().PlaySound(".\\Assets\\Audio\\jump.mp3", AudioType::EFFECT);
				}
				
				controller.jump_released = false;
			}
		}

		f32 newXVel = move.x * controller.acceleration * dt + body.velocity.x;

		newXVel = newXVel * 0.97f;
		body.velocity.x = newXVel;
		body.velocity.y += move.y;

		//// Dash
		//if (controller.elapsed_time - controller.last_dash_time > 0.5f) {
		//	controller.is_dashing = false;
		//}

		//if (!controller.is_dashing) {
		//	if (System::Input().isKeyTriggered(SDL_SCANCODE_LSHIFT) || System::Input().isControllerKeyTriggered(SDL_CONTROLLER_BUTTON_X)) {
		//		if (body.velocity.x > 0.1f) {
		//			body.velocity.x += 60.0f;
		//			controller.last_dash_time = controller.elapsed_time;
		//			controller.is_dashing = true;
		//		}
		//		if (body.velocity.x < -0.1f) {
		//			body.velocity.x -= 60.0f;
		//			controller.last_dash_time = controller.elapsed_time;
		//			controller.is_dashing = true;
		//		}
		//	}
		//}

		//// Crouch
		//assert(e.Has<Transform>() && "All entities with a controller must have a Transform.");
		//auto& tr = e.Get<Transform>();

		//if (System::Input().isKeyTriggered(SDL_SCANCODE_LCTRL) || System::Input().isControllerKeyTriggered(SDL_CONTROLLER_BUTTON_Y)) {
		//	controller.is_crouched = true;
		//	body.positionOffset.y += tr.scale.y * 0.25f;
		//	body.width.y *= 0.5f;
		//	body.position -= body.positionOffset;
		//}
		//if (System::Input().isKeyReleased(SDL_SCANCODE_LCTRL) || System::Input().isControllerKeyReleased(SDL_CONTROLLER_BUTTON_Y)) {
		//	controller.is_crouched = false;
		//	body.position += body.positionOffset;
		//	body.width.y *= 2.0f;
		//	body.positionOffset.y -= tr.scale.y * 0.25f;
		//}

		//// Hide
		//if (System::Input().isKeyTriggered(SDL_SCANCODE_H)) {
		//	controller.is_hiding = !controller.is_hiding;

		//	if (e.Has<AABB>()) {
		//		auto& collider = e.Get<AABB>();

		//		// hack. set width < 0 to turn off collider
		//		if (controller.is_hiding)
		//			collider.rx *= collider.rx > 0.0f ? -1.0f : 1.0f;
		//		else
		//			collider.rx *= collider.rx > 0.0f ? 1.0f : -1.0f;
		//	}
		//}

		//// Shoot Bouncy Ball
		//if (System::Input().isMouseButtonTriggered(SDL_BUTTON_RMASK)) {
		//	vec3 mouse = vec3{ 0.0f };
		//	mouse.x = System::Input().GetMouseWorldPosition().x;
		//	mouse.y = System::Input().GetMouseWorldPosition().y;

		//	System::Events().EnqueueEvent(EventType::WeaponFired, 0.0f, {
		//		{"EntityID_player", e},
		//		{"f32_mouseX", mouse.x },
		//		{"f32_mouseY", mouse.y}
		//	});
		//}

	}

	void SceneChangeSystem(CollidingWith& cw)
	{
		Entity owner = System::ECS().GetOwner(cw);
		assert(owner.Has<PlayerController>() && "Only the player should have CollidingWith component");
		
		for (auto const& eid : cw.entity_ids)
		{
			if (System::ECS().IsValidEntity(eid))
			{
				Entity other = System::ECS().GetEntityFromID(eid);
				if (other.Has<SceneChangeTrigger>())
				{
					auto const& sct = other.Get<SceneChangeTrigger>();

					System::App().prevScene = System::App().currentScene;
					System::App().currentScene = sct.nextScene.Id();

					System::App().currentState = Application::State::SceneOut;
					
					System::Cam().ToggleInvertedColors();
					System::Audio().PlaySound(".\\Assets\\Audio\\light_switch.mp3", sic::AudioType::EFFECT);
					assert(other.Has<FadeScreen>());
					auto& fader = other.Get<FadeScreen>();
					fader.fadeIn = false;
					
					break;
				}
			}
		}
	}

	void ThrowSystem(Grabber& gbr) {
		if (gbr.grabbing == true && System::ECS().IsValidEntity(gbr.grabbed_entity)) {
			if (System::Input().isMouseButtonPressed(SDL_BUTTON_LMASK)) {
				if (gbr.throw_power < 1.0f) {
					gbr.throw_power += 0.05f;
				}
			}
			if (System::Input().isMouseButtonReleased(SDL_BUTTON_LMASK)) {
				Entity e = System::ECS().GetEntityFromID(gbr.grabbed_entity);

				Body& body = e.Get<Body>();
				vec2 direction = System::Input().GetMouseWorldVector();
				direction = glm::normalize(direction);

				vec2 playerDir = (body.forward) ? vec2(1, 0) : vec2(-1, 0);
				if (dot(direction, playerDir) >= 0)
				{
					gbr.throw_power += 1.0f;
					direction *= 20.0f * gbr.throw_power;
					body.velocity += direction;

					Grabbable& g = e.Get<Grabbable>();
					g.isGrabbed = false;

					gbr.throw_power = 0.0f;
					gbr.grabbing = false;
					gbr.grabbed_entity = NullEntityId;
				}
			}
		}
	}

	void GrabSystem(Grabber& gbr)
	{
		Entity owner = System::ECS().GetOwner(gbr);
		assert(owner.Has<CollidingWith>());
		auto& cw = owner.Get<CollidingWith>();
		assert(owner.Has<Body>());
		auto& body = owner.Get<Body>();

		if (gbr.grabbing == false) {
			// Check for grab
			if (System::Input().isKeyTriggered(SDL_SCANCODE_G)) {
				for (auto& eid : cw.entity_ids) {
					if (System::ECS().IsValidEntity(eid)) {
						Entity e = System::ECS().GetEntityFromID(eid);
						if (e.Has<Grabbable>()) {
							gbr.grabbed_entity = eid;
							gbr.grabbing = true;
							auto& gbb = e.Get<Grabbable>();
							gbb.isGrabbed = true;
							break;
						}
					}
				}
			}
		}
		else {
			// Adjust position of grabbed object
			Entity e = System::ECS().GetEntityFromID(gbr.grabbed_entity);
			assert(e.Has<Body>());
			auto& body_g = e.Get<Body>();
			body_g.velocity = vec2{ 0.0f };
			body_g.angularVelocity = 0.0f;
			body_g.rotation = 0.0f;
			body_g.position.y = body.position.y + body.width.y * 0.5f;
			if (body.forward == true) {
				body_g.position.x = body.position.x + body.width.x * 0.5f + body_g.width.x * 0.5f + 0.05f;
			}
			else {
				body_g.position.x = body.position.x - body.width.x * 0.5f - body_g.width.x * 0.5f - 0.05f;
			}

			// Check for release
			if (System::Input().isKeyTriggered(SDL_SCANCODE_G)) {
				auto& gbb = e.Get<Grabbable>();
				gbb.isGrabbed = false;
				gbr.grabbed_entity = NullEntityId;
				gbr.grabbing = false;
			}
		}
	}

	void CheatCodeSystem(FadeScreen& fs)
	{	
		StringId levelId;
		switch (fs.code_index)
		{
		case 0:
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_L))
				fs.code_index = 1;
			break;
		case 1:
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_E))
				fs.code_index = 2;
			break;
		case 2:
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_V))
				fs.code_index = 3;
			break;
		case 3:
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_E))
				fs.code_index = 4;
			break;
		case 4:
			if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_L))
				fs.code_index = 5;
			break;
		case 5:
			if (System::Input().isKeyTriggered(SDL_SCANCODE_0)) {
				fs.code_index = 6;
				levelId = "TutorialLevel.json"_sid;
			}
			else if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_1)) {
				fs.code_index = 6;
				levelId = "Level1.json"_sid;
			}
			else if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_2)) {
				fs.code_index = 6;
				levelId = "Level2.json"_sid;
			}
			else if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_3)) {
				fs.code_index = 6;
				levelId = "Level3.json"_sid;
			}
			else if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_4)) {
				fs.code_index = 6;
				levelId = "BedroomLevel.json"_sid;
			}
			else if (sic::System::Input().isKeyTriggered(SDL_SCANCODE_5)) {
				fs.code_index = 6;
				levelId = "KitchenLevel.json"_sid;
			}
			break;
		}

		if (fs.code_index == 6)
		{			
			System::App().currentState = Application::State::SceneOut;

			System::App().prevScene = System::App().currentScene;
			System::App().currentScene = levelId.Id();

			System::Cam().ToggleInvertedColors();
						
			fs.fadeIn = false;
		}
	}

}