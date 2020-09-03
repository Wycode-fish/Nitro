#include "NtPCH.h"
#include "Renderer.h"

namespace Nitro
{
	namespace Graphics
	{
		void Renderer::BeginScene()
		{
		}
		void Renderer::EndScene()
		{
		}
		void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao)
		{
			shader->Bind();
			vao->Bind();
			RenderCommand::DrawIndexed(vao);
		}
	}
}