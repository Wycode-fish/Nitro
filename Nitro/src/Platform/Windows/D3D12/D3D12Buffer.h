#pragma once

#include "Nitro/Render/Buffer.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class vtxbuffer : public VertexBuffer
			{
			public:
				vtxbuffer(float* data, const size_t& dataSz);
			public:
				virtual ~vtxbuffer();
				virtual void Bind() const;
				virtual void Unbind() const;
				virtual void SetLayout(const BufferLayout& layout);
				virtual inline const BufferLayout& GetLayout() const override
				{
					return m_Layout;
				}
				
			private:
				BufferLayout				m_Layout;
			};
		}
	}
}