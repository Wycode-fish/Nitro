#pragma once
#include "Nitro/Render/RendererAPI.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class D3D12RendererAPI : public RendererAPI
			{
			public:
				virtual void Clear();
				virtual void SetClearColor(const glm::vec4& color);
				virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vao);
			};
		}
	}
}