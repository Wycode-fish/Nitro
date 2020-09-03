#include "NtPCH.h"
#include "RenderCommand.h"

#ifdef NT_WINDOWED_APP
#include "Platform/Windows/D3D12/D3D12RendererAPI.h"
#else
#include "Platform/Independent/OpenGL/OpenGLRendererAPI.h"
#endif

namespace Nitro
{
	namespace Graphics
	{
#ifdef NT_WINDOWED_APP
		RendererAPI* RenderCommand::s_RendererAPI = new dx::D3D12RendererAPI();
#else
		RendererAPI* RenderCommand::s_RendererAPI = new gl::OpenGLRendererAPI();
#endif

		void RenderCommand::ClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		void RenderCommand::Clear()
		{
			s_RendererAPI->Clear();
		}

		void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vao)
		{
			s_RendererAPI->DrawIndexed(vao);
		}
	}
}
