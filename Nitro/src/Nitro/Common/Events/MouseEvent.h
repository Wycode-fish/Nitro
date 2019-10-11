#pragma once

#include "Event.h"

namespace Nitro
{
	namespace Base
	{
		typedef unsigned char TButton;

		class NITRO_API MouseMovedEvent : public Event
		{
		public:
			MouseMovedEvent(float x, float y)
				: m_PosX{ x }, m_PosY{ y }
			{}
			inline float GetPosX() const { return m_PosX; }
			inline float GetPosY() const { return m_PosY; }
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseMovedEvent: x=" << m_PosX << ", y=" << m_PosY;
				return ss.str();
			}
			EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
				EVENT_CLASS_TYPE(MouseMoved)
		private:
			float m_PosX, m_PosY;
		};

		class NITRO_API MouseScrolledEvent : public Event
		{
		public:
			MouseScrolledEvent(float offset_x, float offset_y)
				: m_OffsetX{ offset_x }, m_OffsetY{ offset_y }
			{}
			inline float GetOffsetX() const { return m_OffsetX; }
			inline float GetOffsetY() const { return m_OffsetY; }
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseScrolledEvent: x=" << m_OffsetX << ", y=" << m_OffsetY;
				return ss.str();
			}
			EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
				EVENT_CLASS_TYPE(MouseScrolled)
		private:
			float m_OffsetX, m_OffsetY;
		};

		class NITRO_API MouseButtonEvent : public Event
		{
		public:
			inline TButton GetButton() const { return m_Button; }
			EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton)
		protected:
			MouseButtonEvent(TButton button)
				: m_Button{ button }
			{}
			TButton m_Button;
		};

		class NITRO_API MouseButtonPressedEvent : public MouseButtonEvent
		{
		public:
			MouseButtonPressedEvent(TButton button)
				: MouseButtonEvent{ button }
			{}
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MousePressedEvent: button-" << m_Button;
				return ss.str();
			}
			EVENT_CLASS_TYPE(MouseButtonPressed)
		};

		class NITRO_API MouseButtonReleasedEvent : public MouseButtonEvent
		{
		public:
			MouseButtonReleasedEvent(TButton button)
				: MouseButtonEvent{ button }
			{}
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MousePressedEvent: button-" << m_Button;
				return ss.str();
			}
			EVENT_CLASS_TYPE(MouseButtonReleased)
		};
	}
}