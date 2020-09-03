#pragma once
#include "glm/glm.hpp"
#include "VertexArray.h"

namespace Nitro
{
	namespace Graphics
	{
		class RendererAPI
		{
		public:
			enum class API
			{
				None = 0, OpenGL, D3D12
			};
			static inline API GetAPI() { return s_API; }
		public:
			virtual void Clear()												= 0;
			virtual void SetClearColor(const glm::vec4& color)					= 0;
			virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vao)	= 0;
		private:
			static API s_API;
		};
	}
}