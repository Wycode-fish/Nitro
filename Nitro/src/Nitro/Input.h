#pragma once

#include "Core.h"

namespace Nitro
{
	namespace Base
	{
		// Interface for platform-dependent Input class
		class NITRO_API Input
		{
		protected:
			typedef unsigned char TButton;
			typedef int TKeyCode;

		public:
			static inline bool IsKeyPressed(TKeyCode keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
			static inline bool IsMouseButtonPressed(TButton button) { return s_Instance->IsMouseButtonPressedImpl(button); }
			static inline std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
			static inline float GetMouseX() { return s_Instance->GetMouseXImpl(); }
			static inline float GetMouseY() { return s_Instance->GetMouseYImpl(); }

		protected:
			virtual bool IsKeyPressedImpl(TKeyCode keycode) const = 0;
			virtual bool IsMouseButtonPressedImpl(TButton button) const = 0;
			virtual std::pair<float, float> GetMousePositionImpl() const = 0;
			virtual float GetMouseXImpl() const = 0;
			virtual float GetMouseYImpl() const = 0;

		private:
			static Input* s_Instance;
		};
	}
}