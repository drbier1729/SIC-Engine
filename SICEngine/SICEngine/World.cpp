/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "World.h"

#include "Entities.h"
#include "Components.h"
#include "Factory.h"
//#include "SystemsManager.h"
#include "EventManager.h"

namespace sic {

	World::World(EntityAdmin* ecs_, Factory* factory_, /*SystemManager* systems_,*/ EventManager* events_)
		: m_ecs{ ecs_ },
		m_factory{ factory_ }, 
		/*m_systems{ systems_ },*/ 
		m_events{ events_ }
	{ }
}