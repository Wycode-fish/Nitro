#include "NtPCH.h"
#include "WindowsInput.h"

#include "Nitro/Application.h"
#include <GLFW/glfw3.h>

namespace Nitro
{
	namespace Base
	{
		Input* Input::s_Instance = new WindowsInput();

		bool WindowsInput::IsKeyPressedImpl(TKeyCode keycode) const
		{
			GLFWwindow* wnd = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow()->GetNativeWindow());
			int state = glfwGetKey(wnd, keycode);
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}
		bool WindowsInput::IsMouseButtonPressedImpl(TButton button) const
		{
			GLFWwindow* wnd = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow()->GetNativeWindow());
			int state = glfwGetMouseButton(wnd, button);
			return state == GLFW_PRESS;
		}
		std::pair<float, float> WindowsInput::GetMousePositionImpl() const
		{
			GLFWwindow* wnd = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow()->GetNativeWindow());
			double x, y;
			glfwGetCursorPos(wnd, &x, &y);
			return { x, y };
		}
		float WindowsInput::GetMouseXImpl() const
		{
			auto[x, y] = GetMousePositionImpl();
			return x;
		}
		float WindowsInput::GetMouseYImpl() const
		{
			auto[x, y] = GetMousePositionImpl();
			return y;
		}
	}
}
