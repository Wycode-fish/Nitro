#include "NtPCH.h"
#include "Buffer.h"
#include "Renderer.h"

#ifdef NT_PLATFORM_WINDOWS
#include "Platform/Windows/D3D12/D3D12Buffer.h"
#include "Platform/Independent/OpenGL/OpenGLBuffer.h"
#endif

namespace Nitro
{
	namespace Graphics
	{
		// @ -------------------------------------------------------------
		// @						VERTEXBUFFER
		// @ -------------------------------------------------------------
		VertexBuffer* VertexBuffer::Create(float* data, const size_t& dataSz)
		{
			RendererAPI::API api = Renderer::GetAPI();
			switch (api)
			{
			case RendererAPI::API::None:
				NT_ASSERT(false, "RenderAPI::None is not supported.");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return new gl::vtxbuffer(data, dataSz);
			case RendererAPI::API::D3D12:
				return new dx::vtxbuffer(data, dataSz);
			}

			NT_ASSERT(false, "Invalid RendererAPI.");
			return nullptr;
		}

		// @ -------------------------------------------------------------
		// @						INDEXBUFFER
		// @ -------------------------------------------------------------
		IndexBuffer* IndexBuffer::Create(u32* data, const size_t& cnt)
		{
			RendererAPI::API api = Renderer::GetAPI();
			switch (api)
			{
			case RendererAPI::API::None:
				NT_ASSERT(false, "RenderAPI::None is not supported.");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return new gl::idxbuffer(data, cnt);
			}

			NT_ASSERT(false, "Invalid RendererAPI.");
			return nullptr;
		}
	}
}