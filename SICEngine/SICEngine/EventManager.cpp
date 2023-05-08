/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include "EventManager.h"

#include <cassert>

#include "Events.h"
#include "Entities.h"
#include "Components.h"

// ============
// EventManager
// ============
namespace sic {
	EventManager::EventManager() : EventManager(allocator_type{}) {}
	
	EventManager::EventManager(allocator_type alloc)
		: m_queues(NUM_EVENTS, alloc), 
		m_subscriberTable(NUM_EVENTS, alloc), 
		m_eventHandlers{ alloc },
		m_internalTimer{0.0f}
	{
#define REGISTER_EVENT(type)
#define REGISTER_HANDLER(name) m_eventHandlers.emplace_back(StringId{#name}, name);
#include "Events.def" 
#undef REGISTER_HANDLER
#undef REGISTER_EVENT
	}

	EventManager::~EventManager()
	{
		Reset();
	}

	void EventManager::Reset(void)
	{
		// Remove lingering events in all queues, and reset cached iterators
		for (auto& queue : m_queues)
		{
			queue.clear();
			queue.iter_ongoingEnd = queue.events.end();
		}

		// Reset the event timer
		m_internalTimer = 0.0f;
	}

	void EventManager::Subscribe(EventType type, StringId::underlying_type handlerName_sid)
	{
		// Find handler
		auto it = m_eventHandlers.begin();
		for (; it != m_eventHandlers.end(); it++)
		{
			if (it->name == handlerName_sid) { break; }
		}
		assert(it != m_eventHandlers.end() && "No handler found with this name.");
		
		// Subscribe to event type
		EventID id{ static_cast<EventID>(type) };
		assert(id < m_subscriberTable.size());

		m_subscriberTable[id].emplace_back(*it);
	}

	void EventManager::Subscribe(EventType type, const char* handlerName)
	{
		Subscribe(std::forward<EventType>(type), StringId{ handlerName });
	}

	void EventManager::Subscribe(EventType type, StringId handlerName_sid)
	{
		Subscribe(std::forward<EventType>(type), handlerName_sid.Id());
	}

	void EventManager::Unsubscribe(EventType type, StringId::underlying_type handlerName_sid)
	{
		EventID id{ static_cast<EventID>(type) };
		assert(id < m_subscriberTable.size());

		auto& subscribers = m_subscriberTable[id];
		assert(!subscribers.empty() && "Cannot unsubscribe from an empty list.");

		auto it = subscribers.begin();
		for (; it != subscribers.end(); it++)
		{
			if (it->name == handlerName_sid) { break; }
		}
		assert(it != subscribers.end() && "Handler is not subscribed to this EventType.");

		std::iter_swap(it, subscribers.end() - 1);
		subscribers.pop_back();
	}

	void EventManager::Unsubscribe(EventType type, const char* handlerName)
	{
		Unsubscribe(std::forward<EventType>(type), StringId{ handlerName });
	}

	void EventManager::Unsubscribe(EventType type, StringId handlerName_sid)
	{
		Unsubscribe(std::forward<EventType>(type), handlerName_sid.Id());
	}

	
	void EventManager::Dispatch(EventType type)
	{
		EventID id{ static_cast<EventID>(type) };

		assert(id < m_queues.size() && id < m_subscriberTable.size());

		EventQueue& queue{ m_queues[id] };
		std::pmr::vector<EventHandler>& handlers{ m_subscriberTable[id] };

		for (auto& h : handlers)
		{
			std::for_each(queue.events.begin(), queue.iter_ongoingEnd, h.handler);
		}

		queue.Pump(std::forward<f32>(m_internalTimer));
	}

	void EventManager::EnqueueEvent(EventType type, f32 timer_sec, std::initializer_list<std::pair<const char*, EventArg>> args)
	{
		EventID id{ static_cast<EventID>(type) };

		assert(id < m_queues.size());

		m_queues[id].Push(std::forward<f32>(m_internalTimer),
			std::forward<EventType>(type),
			std::forward<f32>(timer_sec),
			std::forward<std::initializer_list<std::pair<const char*, EventArg>>>(args));
	}

	void EventManager::Tick(f32 dt) { m_internalTimer += dt; }

}

// =========
// EventArgs
// =========
namespace sic {

	EventArg::EventArg(bool8 b) : type{ ArgType::BOOL }, asBool{ b } {}
	EventArg::EventArg(i32 i) : type{ ArgType::I32 }, asI32{ i } {}
	EventArg::EventArg(u32 u) : type{ ArgType::U32 }, asU32{ u } {}
	EventArg::EventArg(f32 f) : type{ ArgType::F32 }, asF32{ f } {}
	EventArg::EventArg(i64 i) : type{ ArgType::I64 }, asI64{ i } {}
	EventArg::EventArg(f64 f) : type{ ArgType::F64 }, asF64{ f } {}
	EventArg::EventArg(u64 u) : type{ ArgType::U64 }, asU64{ u } {}
	EventArg::EventArg(Entity const& e) : type{ ArgType::ENTITY_ID }, asU64{ e.Id() } {}
}

// ======
// Events
// ======
namespace sic {

	Event::Event(EventType _type, f32 _when_sec, std::initializer_list<std::pair<const char*, EventArg>> _args)
		: type{ _type }, num_args{ _args.size() }, when_sec{ _when_sec }
	{
		assert(num_args < max_args);

		size_t i{ 0 };
		for (auto&& a : _args) 
		{ 
			argNames[i] = StringId{ a.first };
			argVals[i] = a.second; 
			++i; 
		}
	}

	EventType Event::Type(void) const { return type; }

	EventArg Event::GetArg(StringId::underlying_type argName_sid) const
	{
		for (size_t i = 0; i < max_args; i++)
		{
			if (argName_sid == argNames[i]) { return argVals[i]; }
		}

		assert(false && "Invalid arg name.");
		return EventArg();
	}

	EventArg Event::GetArg(const char* argName) const 
	{ 
		return std::forward<EventArg>( GetArg(StringId{ argName }) );
	}

	EventArg  Event::GetArg(StringId argName_sid) const
	{
		return std::forward<EventArg>(GetArg(argName_sid.Id()));
	}

	size_t Event::size(void) const { return num_args; }

	f32 Event::when(void) const { return when_sec; }

	bool operator<(Event const& lhs, Event const& rhs) { return lhs.when_sec < rhs.when_sec; }
}

// ==========
// EventQueue
// ==========
namespace sic {
	
	size_t EventQueue::size(void) const { return events.size(); }

	void EventQueue::Push(f32&& currentTime, EventType&& type, f32&& timer_sec, std::initializer_list<std::pair<const char*, EventArg>>&& args)
	{
		assert(events.size() < EventQueue::max_size);

		// Construct the new event in place
		auto const& iter = events.emplace(std::forward<EventType>(type),
			std::forward<f32>(timer_sec + currentTime),
			std::forward<std::initializer_list<std::pair<const char*, EventArg>>>(args));

		// If new event will occur in the future, but sooner than the soonest future event,
		// change the iterator to the end of ongoing events
		if (timer_sec > 0.0f)
		{
			if (iter_ongoingEnd == events.end() || (currentTime + timer_sec) < iter_ongoingEnd->when_sec)
			{
				iter_ongoingEnd = iter;
			}
		}
	}

	void EventQueue::Pump(f32&& currentTime)
	{
		events.erase(events.begin(), iter_ongoingEnd);

		// Compare with a Null Event set for current time to identify the new end iter
		// of the ongoing events (i.e. events with when_sec <= currentTime)
		iter_ongoingEnd = events.upper_bound(Event(EventType::COUNT, std::forward<f32>(currentTime), {}));
	}

	void EventQueue::clear(void) { events.clear(); }


	EventQueue::RawEventQueue const& EventQueue::Events(void) const
	{
		return events;
	}

	std::pair< EventQueue::RawEventQueue const&, EventQueue::RawEventQueue::const_iterator const&>
		EventQueue::OngoingEvents(void) const
	{
		return { events, iter_ongoingEnd };
	}
}
