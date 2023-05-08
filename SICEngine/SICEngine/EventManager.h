/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: -
 */
 

#pragma once

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <cassert>

#include "SICTypes.h"
#include "ComponentBase.h" // used by EventArg
#include "StringId.h"

namespace sic {


	// Forward Declarations
	struct Entity;
	enum class ComponentType : ComponentID;

	enum class EventType : EventID;
	class Event;
	class EventArg;
	class EventQueue;

	struct EventHandler
	{
		StringId name;
		std::function<void(Event const&)> handler;
	};
}


// ============
// EventManager
// ============
namespace sic{

	class EventManager
	{
		friend class ImGuiWindow;

	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;

		EventManager();
		explicit EventManager(allocator_type alloc);
		~EventManager();

		void Subscribe(EventType type, const char* handlerName);
		void Subscribe(EventType type, StringId handlerName_sid);
		void Subscribe(EventType type, StringId::underlying_type handlerName_sid);
		void Unsubscribe(EventType type, const char* handlerName);
		void Unsubscribe(EventType type, StringId handlerName_sid);
		void Unsubscribe(EventType type, StringId::underlying_type handlerName_sid);

		void EnqueueEvent(EventType type, f32 timer_sec, std::initializer_list<std::pair<const char*, EventArg>> args);
		void Dispatch(EventType);

		void Tick(f32 dt);

		void Reset(void);

	public:
		EventManager(EventManager const& src, allocator_type alloc = {}) = delete;
		EventManager(EventManager&& src) = delete;
		EventManager(EventManager&& src, allocator_type alloc) = delete;
		EventManager& operator=(EventManager const& rhs) = delete;
		EventManager& operator=(EventManager&& rhs) = delete;

	private:
		// Indexed by EventID
		std::pmr::vector<EventQueue> m_queues;
		std::pmr::vector<std::pmr::vector<EventHandler>> m_subscriberTable;
		std::pmr::vector<EventHandler> m_eventHandlers;

		f32 m_internalTimer;
	};
}
	
// ========
// EventArg
// ========
namespace sic {
	// EventArg is a "safe" union which can be read as integer, unsigned integer, float, 
	// bool, ComponentID, or EntityID. Used as arguments passed in Events.
	class EventArg
	{
	public:
		enum class ArgType
		{
			I32,
			U32,
			I64,
			U64,
			F32,
			F64,
			BOOL,
			COMPONENT_ID,
			ENTITY_ID,
			NUM
		};

		// Default ctor
		EventArg() : type{ ArgType::U64 }, asI64{} {}

		// Conversion ctors
		EventArg(bool8 b);
		EventArg(i32 i);
		EventArg(u32 u);
		EventArg(f32 f);
		EventArg(i64 i);
		EventArg(f64 f);
		EventArg(u64 u);
		EventArg(Entity const& e);
		template<ValidComponent C> EventArg(C const& c);

		// Accessors
		bool8		AsBool(void) const { assert(type == ArgType::BOOL); return asBool; }
		i32			AsI32(void) const { assert(type == ArgType::I32); return asI32; }
		u32			AsU32(void) const { assert(type == ArgType::U32); return asU32; }
		f32			AsF32(void) const { assert(type == ArgType::F32); return asF32; }
		i64			AsI64(void) const { assert(type == ArgType::I64); return asI64; }
		u64			AsU64(void) const { assert(type == ArgType::U64); return asU64; }
		f64			AsF64(void) const { assert(type == ArgType::F64); return asF64; }
		ComponentID AsComponentID(void) const { assert(type == ArgType::COMPONENT_ID); return asU64; }
		EntityID	AsEntityID(void) const { assert(type == ArgType::ENTITY_ID); return asU64; }

		ArgType		Type(void) { return type; }

	private:
		ArgType type;
		union
		{
			bool8 asBool;
			i32  asI32;
			u32  asU32;
			f32  asF32;
			i64	 asI64;
			u64	 asU64;
			f64	 asF64;
		};
	};
}

// =====
// Event
// =====
namespace sic { 
	class Event
	{
		friend class EventQueue;
		friend bool operator<(Event const&, Event const&);

	public:
		Event(EventType type, f32 when_sec, std::initializer_list < std::pair<const char*, EventArg> > args);

		// Accessors
		EventType Type(void) const;
		EventArg  GetArg(const char* argName) const;
		EventArg  GetArg(StringId argName_sid) const;
		EventArg  GetArg(StringId::underlying_type argName_sid) const;
		size_t    size(void) const;
		f32		  when(void) const;

	private:
		class_global constexpr size_t max_args{ 8 };

		std::array<StringId, max_args>  argNames; // TODO (dylan) : should this be StringId::underlying_type? No. We want to see the names in debug.
		std::array<EventArg, max_args>  argVals;
		
		size_t    num_args;

		f32       when_sec; // timestamp of when the event should occur

		EventType type;
	};

	bool operator<(Event const& lhs, Event const& rhs);
}


// ==========
// EventQueue
// ==========
namespace sic {

	// Wrapper for a container of Events sorted by time remaining on their timer
	class EventQueue
	{
	public:
		friend class EventManager;

		// TODO : this is pretty awful... we would prefer a circular buffer for events and ~something else~ for messages
		// (i.e. what we're calling timed events now)
		using RawEventQueue = std::pmr::multiset<Event>;

		using allocator_type = std::pmr::polymorphic_allocator<>;

	public:
		EventQueue() : EventQueue(allocator_type{}) {}
		explicit EventQueue(allocator_type alloc) : events{ alloc }, iter_ongoingEnd{ events.end() } {}

		size_t size(void) const;
		void Push(f32&& currentTime, EventType&& type, f32&& timer_sec, std::initializer_list < std::pair<const char*, EventArg> > && args);

		RawEventQueue const& Events(void) const;
		std::pair<RawEventQueue const&, RawEventQueue::const_iterator const&> OngoingEvents(void) const;

	private:
		void Pump(f32&& currentTime);
		void clear(void);

	private:
		// maximum number of events that can be held in this queue
		class_global constexpr size_t max_size{ 512 };

		RawEventQueue events;
		RawEventQueue::const_iterator iter_ongoingEnd;
	};

}

#include "EventManager.tpp"