#include "NtPCH.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#ifdef NT_WINDOWED_APP
// ** necessary Win32 & d3d12 dependencies are already in pch
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx12.h"
#include "Platform/Windows/D3D12/D3D12Context.h"
#include "Platform/Windows/D3D12/D3D12SwapChain.h"
#include "Platform/Windows/D3D12/D3D12DescriptorHeap.h"
#include "Platform/Windows/D3D12/D3D12CommandContext.h"
#else
#include <GLFW/glfw3.h>
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#endif

#include "Nitro/Application.h"

// @ -------- For Debugging --------------
#ifdef NT_WINDOWED_APP
	#define NT_IMGUI_D3D12_READY 1
#endif
// @ -----------------------------------

namespace Nitro
{
	namespace Framework
	{
#ifdef NT_WINDOWED_APP
		Graphics::dx::D3D12DescriptorHeap_Static* ImGuiLayer::sm_ImGuiSRVHeap = nullptr;
#endif
		ImGuiLayer::ImGuiLayer()
			: m_Now{0.0f}
		{
		}

		void ImGuiLayer::OnAttach()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#ifdef NT_WINDOWED_APP
	#if NT_IMGUI_D3D12_READY > 0
			Application& app = Application::GetInstanceAsRef();
			HWND window = static_cast<HWND>(app.GetWindow()->GetNativeWindow());

			if (!ImGuiLayer::sm_ImGuiSRVHeap)
			{
				ImGuiLayer::sm_ImGuiSRVHeap = nitro_new Graphics::dx::D3D12DescriptorHeap_Static(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
				ImGuiLayer::sm_ImGuiSRVHeap->Init("ImGui SRV Heap", D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
			}

			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX12_Init(
				Nitro::Graphics::dx::D3D12Context::g_Device, 
				Nitro::Graphics::dx::D3D12SwapChain::GetInstance()->GetFrameBufferCount(),
				DXGI_FORMAT_R8G8B8A8_UNORM,
				ImGuiLayer::sm_ImGuiSRVHeap->GetHandleAtOffset(0).CpuHandle,
				ImGuiLayer::sm_ImGuiSRVHeap->GetHandleAtOffset(0).GpuHandle);
	#endif
#else
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// ImGui is not ready to do multi-viewport in d3d12
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			Application& app = Application::GetInstanceAsRef();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeWindow());

			//io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
			//io.BackendFlags |= ImGuiBackendFlags_HasSetMouse;
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
#endif
		}

		void ImGuiLayer::OnDetach()
		{
#ifdef NT_WINDOWED_APP
#if NT_IMGUI_D3D12_READY > 0
			ImGui_ImplDX12_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
#endif
#else
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
#endif
		}

		void ImGuiLayer::Begin()
		{
#ifdef NT_WINDOWED_APP
#if NT_IMGUI_D3D12_READY > 0
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif
#else
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif
		}
		
		void ImGuiLayer::End()
		{
#ifdef NT_WINDOWED_APP
#if NT_IMGUI_D3D12_READY > 0
			Graphics::dx::D3D12ColorBuffer& currentFrameBuffer = Graphics::dx::D3D12SwapChain::GetInstance()->GetCurrentFrameBufferRef();

			Graphics::dx::D3D12CommandContext_Graphics& ctx = Graphics::dx::D3D12CommandContext_Graphics::Begin();
			{
				ctx.TransitionResource(currentFrameBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
				ctx.SetRenderTarget(currentFrameBuffer.RTV);
				//ctx.ClearColor(currentFrameBuffer);
				ctx.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, ImGuiLayer::sm_ImGuiSRVHeap->GetNativeHeap());
				// ----- ImGui Render -----
				ImGui::Render();
				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), ctx.GetCommandList());
				// ------------------------
				ctx.TransitionResource(currentFrameBuffer, D3D12_RESOURCE_STATE_PRESENT);
			}
			ctx.Finish(true);
#endif
#else
			ImGuiIO& io = ImGui::GetIO();
			Application& app = Application::GetInstanceAsRef();
			io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow()->GetWidth()), static_cast<float>(app.GetWindow()->GetHeight()));

			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// ImGui is not ready to do multi-viewport in d3d12, so this part is missing in d3d12 setting
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
#endif
		}

		void ImGuiLayer::OnImGuiRender()
		{
#ifdef NT_WINDOWED_APP
#if NT_IMGUI_D3D12_READY > 0
			// TODO: Implement D3D12 first...
			static bool show = true;
			ImGui::ShowDemoWindow();
#endif
#else
			static bool show = true;
			ImGui::ShowDemoWindow();
#endif
		}
	}
}