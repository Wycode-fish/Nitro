#include "NtPCH.h"
#include "RendererAPI.h"

namespace Nitro
{
	namespace Graphics
	{
#ifdef NT_WINDOWED_APP
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::D3D12;
#else
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif
	}
}