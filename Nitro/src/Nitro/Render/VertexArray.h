#pragma once

#include "Nitro/Render/Buffer.h"
#include <memory>

namespace Nitro
{
	namespace Graphics
	{
		class VertexArray
		{
		public:
			static VertexArray* Create();
			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) = 0;
			virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo) = 0;

			virtual const std::vector<std::weak_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
			virtual const std::weak_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

		};
	}
}