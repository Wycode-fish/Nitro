#pragma once

#include "Nitro/Render/Buffer.h"
#include "Platform/Windows/D3D12/D3D12GpuResource.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class vtxbuffer : public VertexBuffer, public D3D12GpuResource
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
				
				inline const D3D12_VERTEX_BUFFER_VIEW GetVboView() const 
				{
					return m_View;
				}
			private:
				BufferLayout				m_Layout;
				D3D12_VERTEX_BUFFER_VIEW	m_View;
			};
		}
	}
}