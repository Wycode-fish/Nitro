#pragma once
#include <glad/glad.h>
#include "Nitro/Render/Buffer.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			// @ -------------------------------------------------------------
			// @						VERTEXBUFFER
			// @ -------------------------------------------------------------
			class vtxbuffer : public VertexBuffer
			{
			public:
				vtxbuffer(float* vboData, const size_t& dataSz);
				virtual ~vtxbuffer();
			//////////////////
			public:
				virtual void Bind() const override;
				virtual void Unbind() const override;
				virtual inline void SetLayout(const BufferLayout& _layout) override
				{
					layout = _layout;
				}
				virtual inline const BufferLayout& GetLayout() const override
				{
					return layout;
				}
			//////////////////
			private:
				BufferLayout layout;
				u32 rid;
			};

			// @ -------------------------------------------------------------
			// @						INDEXBUFFER
			// @ -------------------------------------------------------------
			class idxbuffer : public IndexBuffer
			{
			public:
				idxbuffer(u32* iboData, const size_t& cnt);
				virtual ~idxbuffer();
			//////////////////
			public:
				virtual void Bind() const override;
				virtual void Unbind() const override;
				virtual inline size_t GetCount() const override 
				{
					return idx_count;
				};
			//////////////////
			private:
				size_t idx_count;
				u32 rid;
			};
		}
	}
}