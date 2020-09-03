#include "NtPCH.h"
#include "VertexArray.h"
#include "Nitro/Render/Renderer.h"
#include "Platform/Independent/OpenGL/OpenGLVertexArray.h"

namespace Nitro
{
	namespace Graphics
	{
		VertexArray* VertexArray::Create()
		{
			RendererAPI::API api = Renderer::GetAPI();
			switch (api)
			{
			case RendererAPI::API::None:
				NT_ASSERT(false, "RenderAPI::None is not supported.");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return new gl::vtxarray();
			}

			NT_ASSERT(false, "Invalid RendererAPI.");
			return nullptr;
		}
	}
}