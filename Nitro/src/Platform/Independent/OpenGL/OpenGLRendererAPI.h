#pragma once

#include "Nitro/Render/RendererAPI.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			class OpenGLRendererAPI : public RendererAPI
			{
			public:
				virtual void Clear() override;
				virtual void SetClearColor(const glm::vec4& color) override;
				virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vao) override;
			};
		}
	}
}