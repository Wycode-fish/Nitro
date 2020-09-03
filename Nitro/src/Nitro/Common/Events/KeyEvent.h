#pragma once

#include "Event.h"

namespace Nitro
{
	namespace Base
	{
		/// KeyEvent is just another interface, stand between Event & KeyXXXEvent.
		class NITRO_API KeyEvent : public Event
		{
		public:
			inline int GetKeyCode() const { return m_KeyCode; }
			EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKey)

		protected:
			KeyEvent(int keyCode)
				: m_KeyCode{ keyCode }
			{}

			int m_KeyCode;
		};

		class NITRO_API KeyPressedEvent : public KeyEvent
		{
		public:
			KeyPressedEvent(int key_code, int repeat_count)
				: KeyEvent{ key_code }, m_RepeatCount{ repeat_count }
			{}
			inline int GetRepeatCount() const { return m_RepeatCount; }
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
				return ss.str();
			}
			EVENT_CLASS_TYPE(KeyPressed)
		private:
			int m_RepeatCount;
		};

		class NITRO_API KeyUpEvent : public KeyEvent
		{
		public:
			KeyUpEvent(int key_code)
				: KeyEvent{ key_code }
			{}
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyUpEvent: " << m_KeyCode;
				return ss.str();
			}
			EVENT_CLASS_TYPE(KeyUp)
		};

		class NITRO_API KeyDownEvent : public KeyEvent
		{
		public:
			KeyDownEvent(int key_code)
				: KeyEvent{ key_code }
			{}
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyDownEvent: " << m_KeyCode;
				return ss.str();
			}
			EVENT_CLASS_TYPE(KeyDown)
		};
	}
}