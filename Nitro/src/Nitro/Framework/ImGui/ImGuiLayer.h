#pragma once

#include "Nitro/Layer.h"
#include "Nitro/Common/Events/KeyEvent.h"
#include "Nitro/Common/Events/MouseEvent.h"
#include "Nitro/Common/Events/ApplicationEvent.h"

namespace Nitro
{
	using namespace Base;

	namespace Framework
	{
		class NITRO_API ImGuiLayer : public Layer
		{
		public:
			ImGuiLayer();
			void OnUpdate() override;
			void OnEvent(Event& ev) override;
			void OnAttach() override;
			void OnDetach() override;

		public:
			bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& ev);
			bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& ev);
			bool OnMouseMovedEvent(MouseMovedEvent& ev);
			bool OnMouseScrolledEvent(MouseScrolledEvent& ev);
			bool OnKeyPressedEvent(KeyPressedEvent& ev);
			bool OnKeyReleasedEvent(KeyReleasedEvent& ev);
			bool OnKeyTypedEvent(KeyTypedEvent& ev);
			bool OnWindowResizedEvent(WindowResizedEvent& ev);

		private:
			float m_Now;
		};
	}
}