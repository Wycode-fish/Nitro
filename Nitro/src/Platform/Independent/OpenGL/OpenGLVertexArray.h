#pragma once
#include <glad/glad.h>
#include "Nitro/Render/VertexArray.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			class vtxarray : public VertexArray
			{
			public:
				vtxarray();
				virtual ~vtxarray();
			public:
				virtual void Bind() const override;
				virtual void Unbind() const override;
				virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) override;
				virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo) override;

				virtual const std::vector<std::weak_ptr<VertexBuffer>>& GetVertexBuffers() const override;
				virtual const std::weak_ptr<IndexBuffer>& GetIndexBuffer() const override;
			private:
				std::vector<std::weak_ptr<VertexBuffer>> vtxbuffers;
				std::weak_ptr<IndexBuffer> idxbuffer;
				u32 rid;
			};
		}
	}
}