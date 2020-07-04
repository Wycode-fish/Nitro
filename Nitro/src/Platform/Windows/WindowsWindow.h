#pragma once

#include "Nitro/Window.h"

#include <GLFW/glfw3.h>

namespace Nitro
{
	namespace Graphics
	{
		class IRenderingContext;
	}
	namespace Base
	{
		class NITRO_API WindowsWindow : public Window
		{
		public:
			WindowsWindow(const WindowBaseProps& props);
			virtual ~WindowsWindow();

			void OnUpdate() override;
			virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
			virtual inline unsigned int GetHeight() const override { return m_Data.Height; }
			virtual inline std::string GetTitle() const override { return m_Data.Title; } 
			virtual inline void* GetNativeWindow() const override { return static_cast<void*>(m_Window); }

			// window attrs
			// inline void SetEventCallbackFunc(const EventCallbackFunc& func) override { m_Data.EventCallback = func; }
			virtual inline void SetEventCallbackFunc(const EventDelegate& func) override { m_Data.EventCallback = func; }
			virtual EventDelegate& GetCallbackFunc() override { return m_Data.EventCallback; }
			virtual void ClearRenderCtx() const override;
			virtual void SetVSync(const bool& enabled) override;
			virtual bool IsVSync() const override;

		protected:
			virtual void WindowInit(const WindowBaseProps& props);
			virtual void GLFWCallbackSetup();
			virtual void WindowShutDown();

		private:
			struct WindowData
			{
				EventDelegate EventCallback;
				std::string Title;
				unsigned int Width, Height;

				bool VSync;
			};

			WindowData m_Data;
			GLFWwindow* m_Window;
		};
	}
}