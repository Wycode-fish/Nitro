#include "NtPCH.h"
#include "WindowsWindow2.h"
#include "Nitro/Application.h"
#include "Nitro/Common/Events/KeyEvent.h"
#include "Nitro/Common/Events/MouseEvent.h"
#include "Nitro/Common/Events/ApplicationEvent.h"
#include "Platform/Windows/D3D12/D3D12Context.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

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
			return 0;
		case WM_KEYUP:
			winDelegate(KeyUpEvent(wParam));
			return 0;
		case WM_MBUTTONDOWN:
			winDelegate(MouseButtonPressedEvent(wParam));
			return 0;
		case WM_CLOSE:	// @ TODO: Probably WM_CLOSE??
			winDelegate(WindowCloseEvent());
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
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
#ifdef NT_WINDOWED_APP
		Window* Window::Create(const WindowBaseProps& props)
		{
			return new WindowsWindow2(props);
		}
#endif
		// @ ------------------------------------------------------
		// @				WindowsWindow2 Impl
		// @ ------------------------------------------------------

		WindowsWindow2::WindowsWindow2(const WindowBaseProps& props)
			: m_Data{props}, m_Window{nullptr}
		{
			m_Context = nitro_new Nitro::Graphics::dx::D3D12Context();
			this->RegisterWindowClass();
			this->CreateWindowInstance();
			// show windows instance
			ShowWindow(m_Window, true);
			UpdateWindow(m_Window);
		}

		WindowsWindow2::~WindowsWindow2()
		{
			DestroyWindow(m_Window);
		}

		void WindowsWindow2::OnUpdate()
		{
			m_Context->SwapBuffers();

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