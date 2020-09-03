#pragma once

#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			// @ -------------------------------------------------------------------
			// @						GpuResource
			// @ -------------------------------------------------------------------
			struct D3D12GpuResource
			{
				D3D12_RESOURCE_STATES		UsageState;
				D3D12_RESOURCE_STATES		TransitioningState;
				D3D12_GPU_VIRTUAL_ADDRESS	GpuAddress;
				ID3D12Resource*				pNativeResource;

#pragma region Constructor/Operator
				D3D12GpuResource();
				D3D12GpuResource(ID3D12Resource* native_resource, D3D12_RESOURCE_STATES current_state);
				ID3D12Resource* operator->();
				ID3D12Resource* const operator->() const;
#pragma endregion

#pragma region Control Methods
				virtual void Destroy();
#pragma endregion
			};

			// @ Define inline in header file like this could be problematic for certain compiler specification
			/*inline ID3D12Resource* D3D12GpuResource::operator->()
			{
				return pNativeResource;
			}*/

			// @ -------------------------------------------------------------------
			// @							GpuBuffer(Interface)
			// @ -------------------------------------------------------------------
			struct D3D12GpuBuffer : public D3D12GpuResource
			{
				enum TypeGpuBuffer
				{
					ByteAddressed,
					Structured,
					Typed,
					NumOfTypes
				};

				D3D12_CPU_DESCRIPTOR_HANDLE UAV;
				D3D12_CPU_DESCRIPTOR_HANDLE SRV;
				size_t						BufferSize;
				u32							ElementCount;
				u32							ElementSize;

				D3D12_RESOURCE_FLAGS		ResourceFlags;
				
				
				void Initialize(u32 numOfElements, u32 elementSize, const void* initData = nullptr, const std::wstring& name = L"");
				virtual ~D3D12GpuBuffer();

#pragma region View Retrievers
				D3D12_VERTEX_BUFFER_VIEW	VertexBufferView(const size_t& offset, u32 size, u32 stride) const;
				D3D12_VERTEX_BUFFER_VIEW	VertexBufferView(const size_t& baseVertexIndex = 0) const;
				D3D12_INDEX_BUFFER_VIEW		IndexBufferView(const size_t& offset, u32 size, bool is32Bit = false) const;
				D3D12_INDEX_BUFFER_VIEW		IndexBufferView(const size_t& startIndex = 0) const;
				D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferView(u32 offset, u32 size) const;
#pragma endregion

#pragma region Interface Methods
			protected:
				virtual void InitializeDerivedViews() = 0;
#pragma endregion

			protected:
				D3D12GpuBuffer();
				D3D12_RESOURCE_DESC DescribeBuffer() const;
				void InitializeDerivedViews(TypeGpuBuffer type, DXGI_FORMAT typedBufferUseOnly = DXGI_FORMAT_UNKNOWN);
			};
		}
	}
}