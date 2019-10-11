#pragma once

#include "Event.h"

namespace Nitro
{
	namespace Base
	{
		class NITRO_API WindowResizedEvent : public Event
		{
		public:
			WindowResizedEvent(unsigned int w, unsigned int h)
				: m_Width{ w }, m_Height{ h }
			{}
			inline unsigned int GetWidth() const { return m_Width; }
			inline unsigned int GetHeight() const { return m_Height; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "WindowResizedEvent: " << m_Width << "(w), " << m_Height << "(h).";
				return ss.str();
			}
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
				EVENT_CLASS_TYPE(WindowResized)
		private:
			unsigned int m_Width, m_Height;
		};

		class NITRO_API WindowCloseEvent : public Event
		{
		public:
			WindowCloseEvent() {}
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
				EVENT_CLASS_TYPE(WindowClose)
		};

		class NITRO_API AppTickEvent : public Event
		{
		public:
			AppTickEvent() {}
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
				EVENT_CLASS_TYPE(AppTick)
		};

		class NITRO_API AppRenderEvent : public Event
		{
		public:
			AppRenderEvent() {}
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
				EVENT_CLASS_TYPE(AppRender)
		};

		class NITRO_API AppUpdateEvent : public Event
		{
		public:
			AppUpdateEvent() {}
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
				EVENT_CLASS_TYPE(AppUpdate)
		};
	}
}