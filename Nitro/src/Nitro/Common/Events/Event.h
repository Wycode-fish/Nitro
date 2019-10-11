#pragma once

#include "Nitro/Core.h"

namespace Nitro
{
	namespace Base
	{
		enum class EventType
		{
			None = 0,
			WindowClose, WindowResized, WindowFocus, WindowLostFocus, WindowMoved,
			AppTick, AppUpdate, AppRender,
			KeyPressed, KeyReleased, KeyTyped,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
		};

		enum EventCategory
		{
			None = 0,
			EventCategoryApplication = BIT(0),
			EventCategoryInput = BIT(1),
			EventCategoryKey = BIT(2),
			EventCategoryMouse = BIT(3),
			EventCategoryMouseButton = BIT(4)
		};
#define EVENT_CLASS_TYPE(type) static EventType s_GetEventType() { return EventType::##type; }\
	EventType GetEventType() const override { return s_GetEventType(); }\
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(categoryFlags) virtual int GetCategoryFlags() const { return categoryFlags; }

		class NITRO_API Event
		{
			friend class EventDispatcher;
		public:
			virtual EventType GetEventType() const = 0;
			virtual int GetCategoryFlags() const = 0;
			virtual const char* GetName() const = 0;
			virtual std::string ToString() const { return GetName(); }
			inline bool IsInCategory(EventCategory cflag) { return cflag & GetCategoryFlags(); }
		public:
			bool Handled = false;
		};

		class NITRO_API EventDispatcher
		{
			template <typename T>
			using EventFunc = std::function<bool(T&)>;
		public:
			EventDispatcher(Event& ev)
				: m_Event{ ev }
			{}
			template<typename T>
			bool Dispatch(EventFunc<T> func)
			{
				if (m_Event.GetEventType() != T::s_GetEventType())
				{
					return false;
				}
				m_Event.Handled = func(*(T*)&m_Event);	// m_Event is just a base type ref, cast to T
				return true;
			}
		private:
			Event& m_Event;
		};

		inline std::ostream& operator << (std::ostream& os, const Event& ev)
		{
			return os << ev.ToString();
		}
	}

}