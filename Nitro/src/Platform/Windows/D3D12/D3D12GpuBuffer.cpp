#include "NtPCH.h"
#include "D3D12GpuBuffer.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12Context.h"
#include "D3D12CommandContext.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			void D3D12ByteAddressedBuffer::InitializeDerivedViews()
			{
				D3D12GpuBuffer::InitializeDerivedViews(TypeGpuBuffer::ByteAddressed);
			}

			void D3D12StructuredBuffer::Destroy()
			{
				this->m_CounterBuffer.Destroy();
				D3D12GpuResource::Destroy();
			}

			void D3D12StructuredBuffer::InitializeDerivedViews()
			{
				D3D12GpuBuffer::InitializeDerivedViews(TypeGpuBuffer::Structured);
			}

			const D3D12_CPU_DESCRIPTOR_HANDLE& D3D12StructuredBuffer::GetCounterSRV(D3D12CommandContext& ctx)
			{
				ctx.TransitionResource(this->m_CounterBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
				return this->m_CounterBuffer.SRV;
			}

			const D3D12_CPU_DESCRIPTOR_HANDLE& D3D12StructuredBuffer::GetCounterUAV(D3D12CommandContext& ctx)
			{
				ctx.TransitionResource(this->m_CounterBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				return this->m_CounterBuffer.UAV;
			}

			D3D12TypedBuffer::D3D12TypedBuffer(DXGI_FORMAT format)
				: Format(format)
			{
			}

			void D3D12TypedBuffer::InitializeDerivedViews()
			{
				D3D12GpuBuffer::InitializeDerivedViews(TypeGpuBuffer::Typed, this->Format);
			}
		}
	}
}

