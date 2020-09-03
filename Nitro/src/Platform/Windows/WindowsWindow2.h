#pragma once
#include "Nitro/Window.h"

extern LRESULT CALLBACK WndProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
);

namespace Nitro
{
	namespace Base
	{
		class NITRO_API  WindowsWindow2 : public Window
		{
		public:
			WindowsWindow2(const WindowBaseProps& props);
		public:
			virtual ~WindowsWindow2();
			virtual void OnUpdate() override;
			virtual inline unsigned int GetWidth() const { return m_Data.Width; }
			virtual unsigned int GetHeight() const { return m_Data.Height; }
			virtual std::string GetTitle() const { return m_Data.Title; }
			virtual void* GetNativeWindow() const {	return m_Window; }

			// @ window attrs
			virtual void SetEventCallbackFunc(const EventDelegate& func) 
			{
				m_Data.EventCallback = func;
			}
			virtual EventDelegate& GetCallbackFunc() override { return m_Data.EventCallback; }
			virtual void SetVSync(const bool& enabled);
			virtual bool IsVSync() const;
		protected:
			void RegisterWindowClass();
			void CreateWindowInstance();
		private:
			struct WindowData : public WindowBaseProps
			{
				EventDelegate EventCallback;
				bool VSync;
				WindowData(const WindowBaseProps& props)
					: WindowBaseProps(props), VSync{false}
				{
				}
			};
			WindowData m_Data;
			HWND m_Window;
		};
	}
}