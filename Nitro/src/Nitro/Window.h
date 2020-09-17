#pragma once

#include "NtPCH.h"
#include "Nitro/Core.h"
#include "Nitro/Common/Events/Event.h"
#include "Nitro/Render/IRenderingContext.h"

#include "Nitro/Util/Delegate.h"

namespace Nitro
{
	namespace Graphics
	{
		class IRenderingContext;
	}
}
namespace Nitro
{
	namespace Base
	{
		struct NITRO_API WindowBaseProps
		{
			std::string Title;
			u32 Width, Height;

			WindowBaseProps(const std::string& title = "NitroEngine", unsigned int width = 1920, unsigned int height = 1080)
				: Title{ title }, Width{ width }, Height{ height }
			{}
			WindowBaseProps(const WindowBaseProps& props)
				: Title{ props.Title }, Width{ props.Width }, Height{ props.Height }
			{}
		};

		class NITRO_API Window
		{
		public:
			// using EventCallbackFunc = std::function<void(Event&)>;
			using EventDelegate = Util::ntDelegate<void(Event&)>;
			virtual ~Window() {}
			virtual void OnUpdate() = 0;
			virtual unsigned int GetWidth() const = 0;
			virtual unsigned int GetHeight() const = 0;
			virtual std::string GetTitle() const = 0;
			virtual void* GetNativeWindow() const = 0;

			// @ window attrs
			// virtual void SetEventCallbackFunc(const EventCallbackFunc& func) = 0;
			virtual void SetEventCallbackFunc(const EventDelegate& func) = 0;
			virtual EventDelegate& GetCallbackFunc() = 0;
			virtual void SetVSync(const bool& enabled) = 0;
			virtual bool IsVSync() const = 0;

			virtual Nitro::Graphics::IRenderingContext* GetContext() { return m_Context; }

			static Window* Create(const WindowBaseProps& props = WindowBaseProps());
		protected:
			Nitro::Graphics::IRenderingContext* m_Context;
		};
	}
}
