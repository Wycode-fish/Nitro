#include "NtPCH.h"
#include "D3D12ReadbackBuffer.h"
#include "D3D12Context.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			void D3D12ReadbackBuffer::Initialize(u32 numOfElements, u32 elementSize, const std::wstring name)
			{
				this->Destroy();

				this->ElementCount = numOfElements;
				this->ElementSize = elementSize;
				this->BufferSize = (size_t)numOfElements * elementSize;

				this->UsageState = D3D12_RESOURCE_STATE_COPY_DEST;

				D3D12_HEAP_PROPERTIES heapProps = {};
				heapProps.Type = D3D12_HEAP_TYPE_READBACK;
				heapProps.CreationNodeMask = 0;
				heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				heapProps.VisibleNodeMask = 0;
				heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

				D3D12_RESOURCE_DESC desc = {};
				desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				desc.Height = 1;
				desc.DepthOrArraySize = 1;
				desc.Width = this->BufferSize;
				desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				desc.MipLevels = 1;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.Flags = D3D12_RESOURCE_FLAG_NONE;
				
				HRESULT hr = D3D12Context::g_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, this->UsageState, nullptr, IID_PPV_ARGS(&this->pNativeResource));
				NT_ASSERT(SUCCEEDED(hr), "Readback Buffer creation failed.");

				this->GpuAddress = this->pNativeResource->GetGPUVirtualAddress();
#ifdef  NT_DEBUG
				this->pNativeResource->SetName(name.c_str());
#endif //  NT_DEBUG
			}

			void* D3D12ReadbackBuffer::Map()
			{
				void* cpuAddr;

				D3D12_RANGE range = {};
				range.Begin = 0;
				range.End = this->BufferSize;

				this->pNativeResource->Map(0, &range, &cpuAddr);
				return cpuAddr;
			}

			void D3D12ReadbackBuffer::Unmap()
			{
				D3D12_RANGE range = {};
				range.Begin = 0;
				range.End = 0;

				this->pNativeResource->Unmap(0, &range);
			}
		}
	}
}