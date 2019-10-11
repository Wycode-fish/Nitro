#include "NtPCH.h"
#include "ImGuiLayer.h"
#include "imgui.h"

#include "Platform/Independent/OpenGL/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Nitro/Application.h"

namespace Nitro
{
	namespace Framework
	{
		ImGuiLayer::ImGuiLayer()
			: m_Now{0.0f}
		{
		}
		void ImGuiLayer::OnUpdate()
		{
			ImGuiIO& io = ImGui::GetIO();
			Base::Application& app = Application::GetInstanceAsRef();
			io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

			float time = (float)glfwGetTime();
			io.DeltaTime = (m_Now == 0.0f) ? 1.0f / 60.0f : time - m_Now;
			m_Now = time;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			static bool showWindow = true;
			ImGui::ShowDemoWindow(&showWindow);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		void ImGuiLayer::OnAttach()
		{
			ImGui::CreateContext();
			ImGui::StyleColorsLight();

			ImGuiIO& io = ImGui::GetIO();
			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

			// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
			io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
			io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
			io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
			io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
			io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
			io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
			io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
			io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
			io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
			io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
			io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
			io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
			io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
			io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
			io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
			io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
			io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
			io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
			io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
			io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
			io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

			ImGui_ImplOpenGL3_Init("#version 410");
		}

		void ImGuiLayer::OnDetach()
		{
		}

		void ImGuiLayer::OnEvent(Event & ev)
		{
			EventDispatcher dispatcher(ev);
			dispatcher.Dispatch<MouseButtonPressedEvent>(NT_EVENT_BIND(&ImGuiLayer::OnMouseButtonPressedEvent));
			dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&ImGuiLayer::OnMouseButtonReleasedEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<MouseMovedEvent>(std::bind(&ImGuiLayer::OnMouseMovedEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&ImGuiLayer::OnMouseScrolledEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&ImGuiLayer::OnKeyReleasedEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<KeyPressedEvent>(std::bind(&ImGuiLayer::OnKeyPressedEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<KeyTypedEvent>(std::bind(&ImGuiLayer::OnKeyTypedEvent, this, std::placeholders::_1));
			dispatcher.Dispatch<WindowResizedEvent>(std::bind(&ImGuiLayer::OnWindowResizedEvent, this, std::placeholders::_1));
		}

		bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDown[ev.GetButton()] = false;
			return false;
		}
		bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDown[ev.GetButton()] = true;
			return false;
		}
		bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MousePos = ImVec2(ev.GetPosX(), ev.GetPosY() );
			return false;
		}
		bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseWheelH += ev.GetOffsetX();
			io.MouseWheel  += ev.GetOffsetY();
			return false;
		}
		bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.KeysDown[ev.GetKeyCode()] = true;

			io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
			io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
			io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];

			return false;
		}
		bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.KeysDown[ev.GetKeyCode()] = false;
			return false;
		}
		bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			unsigned int keyCode = ev.GetKeyCode();
			if (keyCode > 0 && keyCode < 0x10000)
				io.AddInputCharacter((unsigned short)keyCode);
			return false;
		}
		bool ImGuiLayer::OnWindowResizedEvent(WindowResizedEvent & ev)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(ev.GetWidth(), ev.GetHeight());
			return false;
		}
	}
}