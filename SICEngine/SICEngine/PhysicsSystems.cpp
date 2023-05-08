/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Aseem Apastamb
 */
 

#include "ComponentSystems.h"

#include "SICEngine.h"

namespace sic {

	// This should be called whenever we exit the editor or enter a new scene
	void WriteDataToPhysicsWorldSystem(Body& body)
	{
		Entity owner = System::ECS().GetOwner(body);

		assert(owner.Has<Transform>());
		auto const & tr = owner.Get<Transform>();

		body.CalculateImpulse();
		body.UpdatePosAndRot(vec2(tr.position.xy), tr.rotation.z);
		if (!body.setWidthManually) { body.width = vec2(tr.scale.xy); }
	}

	void ReadDataFromPhysicsWorldSystem(Body& body)
	{
		Entity owner = System::ECS().GetOwner(body);

		assert(owner.Has<Transform>());
		auto& tr = owner.Get<Transform>();

		tr.position.x = body.position.x + body.positionOffset.x;
		tr.position.y = body.position.y + body.positionOffset.y;

		tr.rotation.z = body.rotation;
	}
}