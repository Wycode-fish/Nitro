#include "NtPCH.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			void OpenGLRendererAPI::Clear()
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
			{
				glClearColor(color.r, color.g, color.b, color.a);
			}
			void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vao)
			{
				glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer().lock()->GetCount(), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}