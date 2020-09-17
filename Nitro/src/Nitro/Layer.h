#pragma once

namespace Nitro
{
	namespace Base
	{
		class Event;

		class NITRO_API Layer
		{
		public:
			Layer();
			virtual ~Layer();

			virtual void OnAttach() {}
			virtual void OnDetach() {}
			virtual void OnUpdate() {}
			virtual void OnEvent(Event& ev) {}
			virtual void OnImGuiRender() {};
		};
	}
}