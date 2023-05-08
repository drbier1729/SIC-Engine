/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier, Andrew Rudasics
 */
 

#include "Events.h"

#include "SICEngine.h"
#include <SDL_scancode.h> // scancodes for keyboard input

/*
	CollisionEnter Args
	"EntityID_primary"			: EntityID
	"EntityID_secondary"		: EntityID
*/

/*
	CollisionExit Args
	"EntityID_primary"			: EntityID
	"EntityID_secondary"		: EntityID
*/

namespace sic {


	void PlaySoundOnCollisions(Event const& e)
	{
		auto sound_clip = System::Resources().LoadAudioFile(System::Audio().SoundSystem(), ".\\Assets\\Audio\\jump.mp3");
		System::Audio().PlayOnce(sound_clip, AudioType::EFFECT);
	}

	//void CheckForGrab(Event const& e)
	//{
	//	// TODO : this does not currently work because CollisionEvents are not 
		// being enqueued anymore. grabbing should be based on the CollidingWith component.

	//	EntityID a_id = e.GetArg("EntityID_first"_sid).AsEntityID();
	//	EntityID b_id = e.GetArg("EntityID_second"_sid).AsEntityID();

	//	Entity a = System::ECS().GetEntityFromID(a_id);
	//	Entity b = System::ECS().GetEntityFromID(b_id);

	//	assert(a.Has<Transform>() && b.Has<Transform>());
	//	if (a.Has<Grabber>() && b.Has<Grabbable>())
	//	{
	//		bool grab_button = System::Input().isKeyPressed(SDL_SCANCODE_G);

	//		auto& a_gbr = a.Get<Grabber>();
	//		auto& b_gbb = b.Get<Grabbable>();

	//		if (!a_gbr.grabbing && grab_button)
	//		{
	//			a_gbr.grabbing = grab_button;
	//			a_gbr.grabbed = b_id;
	//			a_gbr.offset = (b.Get<Transform>().position - a.Get<Transform>().position).xy;
	//			b_gbb.isGrabbed = true;
	//		}
	//		else if (a_gbr.grabbing && !grab_button)
	//		{
	//			a_gbr.grabbing = grab_button;
	//			a_gbr.grabbed = 0;
	//			a_gbr.offset = glm::vec3(0);
	//			b_gbb.isGrabbed = false;
	//		}
	//	}
	//}

	void UpdateCollidingWithOnCollisionEnter(Event const& e) {
		EntityID a_id = e.GetArg("EntityID_primary"_sid).AsEntityID();
		EntityID b_id = e.GetArg("EntityID_secondary"_sid).AsEntityID();

		Entity a = System::ECS().GetEntityFromID(a_id);
		Entity b = System::ECS().GetEntityFromID(b_id);

		if (a.Has<CollidingWith>()) {
			CollidingWith& cw_a = a.Get<CollidingWith>();
			for (i8 i = 0; i < cw_a.entity_ids.size(); ++i) {
				if (cw_a.entity_ids[i] == NullEntityId) {
					cw_a.entity_ids[i] = b_id;
					if (b.Has<Name>())
					{
						cw_a.names[i] = b.Get<Name>().name;
					}
					else
					{
						cw_a.names[i] = StringId{ "NO NAME" };
					}
					break;
				}
			}
		}

		if (b.Has<CollidingWith>()) {
			CollidingWith& cw_b = b.Get<CollidingWith>();
			for (i8 i = 0; i < cw_b.entity_ids.size(); ++i) {
				if (cw_b.entity_ids[i] == NullEntityId) {
					cw_b.entity_ids[i] = a_id;
					
					if (b.Has<Name>())
					{
						cw_b.names[i] = a.Get<Name>().name;
					}
					else
					{
						cw_b.names[i] = StringId{ "NO NAME" };
					}
					break;
				}
			}
		}
	}

	void UpdateCollidingWithOnCollisionExit(Event const& e) {
		EntityID a_id = e.GetArg("EntityID_primary"_sid).AsEntityID();
		EntityID b_id = e.GetArg("EntityID_secondary"_sid).AsEntityID();

		Entity a = System::ECS().GetEntityFromID(a_id);
		Entity b = System::ECS().GetEntityFromID(b_id);

		if (a.Has<CollidingWith>()) {
			CollidingWith& cw_a = a.Get<CollidingWith>();
			for (i8 i = 0; i < cw_a.entity_ids.size(); ++i) {
				if (cw_a.entity_ids[i] == b_id) {
					cw_a.entity_ids[i] = NullEntityId;
					cw_a.names[i] = "<NONE>"_sid;
					break;
				}
			}
		}

		if (b.Has<CollidingWith>()) {
			CollidingWith& cw_b = b.Get<CollidingWith>();
			for (i8 i = 0; i < cw_b.entity_ids.size(); ++i) {
				if (cw_b.entity_ids[i] == a_id) {
					cw_b.entity_ids[i] = NullEntityId;
					cw_b.names[i] = "<NONE>"_sid;
					break;
				}
			}
		}
	}

}