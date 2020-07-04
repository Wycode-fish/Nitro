#include "NtPCH.h"
#include "WindowsWindow.h"
#include "Nitro/Common/Events/ApplicationEvent.h"
#include "Nitro/Common/Events/KeyEvent.h"
#include "Nitro/Common/Events/MouseEvent.h"
#include "Platform/Independent/OpenGL/GLContext.h"

namespace Nitro
{
	namespace Base
	{
		/// This is made file static instead of WindowsWindow class static
		/// is because we might switch to use Win32 console later with DirectX,
		/// we want the WindowsWindow class independent from the context handle.
		static bool s_IsGLFWInitialized = false;

		/// This is given to glfw as the callback function for error log
		static void GLFWErrorCallback(int error, const char* desc)
		{
			NT_ASSERT(false, "GLFW Error: {0}, {1}", error, desc);
		}

		WindowsWindow::WindowsWindow(const WindowBaseProps& props)
		{
			m_Context = nullptr;
			WindowInit(props);
		}

		WindowsWindow::~WindowsWindow()
		{
			WindowShutDown();
		}

		void WindowsWindow::OnUpdate()
		{
			glfwPollEvents();
			m_Context->SwapBuffers();
		}

		void WindowsWindow::ClearRenderCtx() const
		{
			m_Context->Clear();
		}

		void WindowsWindow::SetVSync(const bool & enabled)
		{
			m_Data.VSync = enabled;
			if (enabled)
				glfwSwapInterval(1);
			else
				glfwSwapInterval(0);
		}

		bool WindowsWindow::IsVSync() const
		{
			return m_Data.VSync;
		}

		void WindowsWindow::WindowInit(const WindowBaseProps& props)
		{
			m_Data.Title = props.Title;
			m_Data.Height = props.Height;
			m_Data.Width = props.Width;

			// @ Initialize GLFW
			if (!s_IsGLFWInitialized)
			{
				int retcode = glfwInit();
				NT_CORE_ASSERT(retcode, "glfw initialization failed.");
				glfwSetErrorCallback(GLFWErrorCallback);
				s_IsGLFWInitialized = true;
			}

			// @ Create Window
			NT_CORE_INFO("creating WindowsWindow ({0}, ({1}, {2}))", props.Title, props.Width, props.Height);
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
			
			// @ Create graphics context
			m_Context = new Graphics::GLContext(m_Window);
			m_Context->Init();

			// @ Setup callbacks for different input
			this->GLFWCallbackSetup();

			// @ Extra setups
			this->SetVSync(true);
		}

		void WindowsWindow::GLFWCallbackSetup()
		{
			// GLFW callback setup
			glfwSetWindowUserPointer(m_Window, &m_Data);

			glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* win, int w, int h)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					dataObj->Height = h;
					dataObj->Width = w;
					WindowResizedEvent ev(w, h);
					dataObj->EventCallback(ev);
				});

			glfwSetCharCallback(m_Window, [](GLFWwindow* win, unsigned int keyCode)->void {
				WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
				KeyDownEvent ev(keyCode);
				dataObj->EventCallback(ev);
				});

			glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* win)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					dataObj->EventCallback(WindowCloseEvent());
				});

			glfwSetKeyCallback(m_Window, [](GLFWwindow* win, int key, int scancode, int action, int mods)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					switch (action)
					{
					case GLFW_PRESS:
					{
						KeyPressedEvent keyPressed(key, 0);
						dataObj->EventCallback(keyPressed);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyUpEvent keyRelease(key);
						dataObj->EventCallback(keyRelease);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent keyRepeat(key, 1);
						dataObj->EventCallback(keyRepeat);
						break;
					}
					}
				});

			glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* win, int button, int action, int mods)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					switch (action)
					{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent buttonPressed(button);
						dataObj->EventCallback(buttonPressed);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent buttonRelease(button);
						dataObj->EventCallback(buttonRelease);
						break;
					}
					}
				});

			glfwSetScrollCallback(m_Window, [](GLFWwindow* win, double offsetX, double offsetY)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					dataObj->EventCallback(MouseScrolledEvent((float)offsetX, (float)offsetY));
				});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* win, double posX, double posY)->void
				{
					WindowData* dataObj = (WindowData*)glfwGetWindowUserPointer(win);
					dataObj->EventCallback(MouseMovedEvent(static_cast<float>(posX), static_cast<float>(posY)));
				});
		}

		void WindowsWindow::WindowShutDown()
		{
			glfwDestroyWindow(m_Window);
		}
	}
}