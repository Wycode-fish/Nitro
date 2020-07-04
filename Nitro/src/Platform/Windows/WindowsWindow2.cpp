#include "NtPCH.h"
#include "WindowsWindow2.h"
#include "Nitro/Application.h"
#include "Nitro/Common/Events/KeyEvent.h"
#include "Nitro/Common/Events/MouseEvent.h"
#include "Nitro/Common/Events/ApplicationEvent.h"

// @ Global Windows Event Handler
static Nitro::Base::Event& WinMsgToNitroEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
	using namespace Nitro::Base;
	// @ TODO: Use Input class implementation to handle
	switch (msg)
	{
	case WM_KEYDOWN:
		return KeyDownEvent(wParam);
	case WM_KEYUP:
		return KeyUpEvent(wParam);
	case WM_MBUTTONDOWN:
		return MouseButtonPressedEvent(wParam);
	case WM_DESTROY:
		return WindowCloseEvent();
	case WM_CANCELMODE:
		return AppUpdateEvent();
	default:
		return AppTickEvent();
	}
	NT_ASSERT(false, "Windows event-message code {0} hasn't been handled by Nitro yet.", msg);
	return WindowCloseEvent();
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	using namespace Nitro::Base;
	// Event& ev = WinMsgToNitroEvent(msg, wParam, lParam);

	// @ -----------------------
	if (Application::GetInstanceAsRef().GetWindow() != nullptr)
	{
		Window::EventDelegate& winDelegate = Application::GetInstanceAsRef().GetWindow()->GetCallbackFunc();
		// @ TODO: Use Input class implementation to handle
		switch (msg)
		{
		case WM_KEYDOWN:
			winDelegate(KeyDownEvent(wParam));
			break;
		case WM_KEYUP:
			winDelegate(KeyUpEvent(wParam));
			break;
		case WM_MBUTTONDOWN:
			winDelegate(MouseButtonPressedEvent(wParam));
			break;
		case WM_DESTROY:
			winDelegate(WindowCloseEvent());
			break;
		}
	}
	// @ -----------------------

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}
namespace Nitro
{
	namespace Base
	{
		/// Note that we have 'Platform' folder outside of 'Nitro' folder, 
		/// which is a signal that, for each platform(win64, macOS, linux etc.), 
		/// 'Nitro' engine code will be separatedly compiled againt them 1 at a time,
		/// so there won't be multiple definitions of Window::Create function.
		Window* Window::Create(const WindowBaseProps& props)
		{
			return new WindowsWindow2(props);
		}

		// @ ------------------------------------------------------
		// @				WindowsWindow2 Impl
		// @ ------------------------------------------------------

		WindowsWindow2::WindowsWindow2(const WindowBaseProps& props)
			: m_Data{props}, m_Window{nullptr}
		{
			m_Context = nullptr;
			this->RegisterWindowClass();
			this->CreateWindowInstance();
			// show windows instance
			ShowWindow(m_Window, true);
			UpdateWindow(m_Window);
		}

		void WindowsWindow2::OnUpdate()
		{
			static MSG msg;
			memset(&msg, 0, sizeof(MSG));

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		void WindowsWindow2::RegisterWindowClass()
		{
			WNDCLASSEX wc;
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.cbClsExtra = NULL;
			wc.cbWndExtra = NULL;
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
			wc.lpszMenuName = NULL;
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);

			wc.lpfnWndProc = WndProc;		// @ TEMPORARY SOLUTION TO DEAL WITH EVENT HERE!!!
			wc.hInstance = GetModuleHandle(NULL);
			std::wstring _className(NT_STDSTR_TO_STDWSTR(m_Data.Title + "Class"));
			wc.lpszClassName = _className.c_str();
			NT_ASSERT(RegisterClassEx(&wc), "window class register failed.");
		}

		void WindowsWindow2::CreateWindowInstance()
		{
			std::wstring _className(NT_STDSTR_TO_STDWSTR(m_Data.Title + "Class"));
			std::wstring _winName(NT_STDSTR_TO_STDWSTR(m_Data.Title));
			m_Window = CreateWindowEx(
				NULL,
				_className.c_str(),
				_winName.c_str(),
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
				m_Data.Width, m_Data.Height,
				NULL, NULL,
				GetModuleHandle(NULL), NULL
			);
			NT_ASSERT(m_Window, "Window instance creation failed.");
		}

		void WindowsWindow2::ClearRenderCtx() const
		{
			// @ TODO: D3D12 clear context...
		}

		void WindowsWindow2::SetVSync(const bool& enabled)
		{
			m_Data.VSync = enabled;
			// @ TODO: D3D12 vsync setting
			// ....
		}

		bool WindowsWindow2::IsVSync() const
		{
			return m_Data.VSync;
		}
	}
}