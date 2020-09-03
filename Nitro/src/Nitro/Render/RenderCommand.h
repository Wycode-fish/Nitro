#pragma once
#include "RendererAPI.h"

namespace Nitro
{
	namespace Graphics
	{
		class RenderCommand
		{
		public:
			static void ClearColor(const glm::vec4& color);
			static void Clear();
			static void DrawIndexed(const std::shared_ptr<VertexArray>& vao);
		private:
			static RendererAPI* s_RendererAPI;
		};
	}
}