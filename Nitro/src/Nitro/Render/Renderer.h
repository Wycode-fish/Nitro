#pragma once

#include "RenderCommand.h"

namespace Nitro
{
	namespace Graphics
	{
		class Renderer
		{
		public:
			static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
			static void BeginScene();
			static void EndScene();
			static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vao);
		};
	}
}