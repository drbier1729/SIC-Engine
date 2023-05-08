/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author:
 */
 
#pragma once

// TODO (dylan) : there isn't much of a point to this since System allows us to get refs to ECS, EventManager and Factory
// Delete?

namespace sic {

	class EntityAdmin;
	class Factory;
	// class SystemsManager;
	class EventManager;

	class World
	{
	public:
		World(EntityAdmin* ecs_ = nullptr, Factory* factory_ = nullptr, /*SystemManager* systems_ = nullptr,*/ EventManager* events_ = nullptr);
		~World() = default;

		inline void Register(EntityAdmin* ecs_) { m_ecs = ecs_; }
		inline void Register(Factory* factory_) { m_factory = factory_; }
		//inline void Register(SystemsManager* systems_) { m_systems = systems_; }
		inline void Register(EventManager* events_) { m_events = events_; }

		inline EntityAdmin&  ecs(void) const { return *m_ecs; }
		inline Factory&		 factory(void) const { return *m_factory; }
		//inline SystemManager& systems(void) const { return *m_systems; }
		inline EventManager& events(void) const { return *m_events; }

	private:
		EntityAdmin* m_ecs;
		Factory* m_factory;
		//SystemManager* m_systems;
		EventManager* m_events;
	};

}