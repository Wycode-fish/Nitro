#pragma once

#include "Nitro/Input.h"

namespace Nitro
{
	namespace Base
	{
		class WindowsInput : public Input
		{
		protected:
			virtual bool IsKeyPressedImpl(TKeyCode keycode) const override;
			virtual bool IsMouseButtonPressedImpl(TButton button) const override;
			virtual std::pair<float, float> GetMousePositionImpl() const override;
			virtual float GetMouseXImpl() const override;
			virtual float GetMouseYImpl() const override;
		};
	}
}