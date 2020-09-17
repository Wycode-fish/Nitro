#include "NtPCH.h"
#include "D3D12GpuResource.h"
#include "D3D12Context.h"
#include "D3D12CommandContext.h"
#include "D3D12DescriptorHeap.h"

#include "Nitro/Util/AlignOps.h"

namespace Nitro
{
    namespace Graphics
    {
        namespace dx
        {
            // @ -------------------------------------------------------------------
            // @						GpuResource
            // @ -------------------------------------------------------------------
            D3D12GpuResource::D3D12GpuResource()
                : UsageState(D3D12_RESOURCE_STATE_COMMON)
                , TransitioningState((D3D12_RESOURCE_STATES)-1)
                , GpuAddress(0)
                , pNativeResource(nullptr)
            {
            }

            D3D12GpuResource::D3D12GpuResource(ID3D12Resource* native_resource, D3D12_RESOURCE_STATES current_state)
                : UsageState(current_state)
                , TransitioningState((D3D12_RESOURCE_STATES)-1)
                , GpuAddress(native_resource->GetGPUVirtualAddress())
                , pNativeResource(native_resource)
            {
            }

            ID3D12Resource* D3D12GpuResource::operator->()
            {
                return pNativeResource;
            }

            ID3D12Resource* const D3D12GpuResource::operator->() const
            {
                return pNativeResource;
            }

            void D3D12GpuResource::Destroy()
            {
                NT_DX_RELEASE(pNativeResource);
                pNativeResource = nullptr;
                GpuAddress = 0;
            }

            void D3D12GpuBuffer::Initialize(u32 numOfElements, u32 elementSize, const void* initData, const std::wstring& name)
            {
                // @ Reset gpu resource.
                this->Destroy();
            
                // @ Creation of Buffer:
                // @    1. Create GpuResource.
                // @    2. Initialize GpuResource
                // @    3. Create descriptor view
                this->ElementCount = numOfElements;
                this->ElementSize = elementSize;
                this->BufferSize = numOfElements * elementSize;
                
                D3D12_RESOURCE_DESC desc = this->DescribeBuffer();

                // @ d3d12 readonly buffer/texture init state: COMMON
                this->UsageState = D3D12_RESOURCE_STATE_COMMON;
                
                D3D12_HEAP_PROPERTIES props = {};
                props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                props.CreationNodeMask = 0;
                props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                props.VisibleNodeMask = 0;
                props.Type = D3D12_HEAP_TYPE_DEFAULT;

                HRESULT hr = D3D12Context::g_Device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, this->UsageState, nullptr, IID_PPV_ARGS(&this->pNativeResource));
                NT_ASSERT(SUCCEEDED(hr), "Creation of gpu buffer resource failed.");

                this->GpuAddress = this->pNativeResource->GetGPUVirtualAddress();
                if (initData != nullptr)
                {
                    D3D12CommandContext::InitializeBuffer(*this, initData, this->BufferSize, 0);
                }
#ifdef NT_DEBUG
                this->pNativeResource->SetName(name.c_str());
#endif // NT_DEBUG
                this->InitializeDerivedViews();
            }

            D3D12GpuBuffer::~D3D12GpuBuffer()
            {
                this->Destroy();
            }

            D3D12_VERTEX_BUFFER_VIEW D3D12GpuBuffer::VertexBufferView(const size_t& offsetInBytes, u32 sizeInBytes, u32 strideInBytes) const
            {
                D3D12_VERTEX_BUFFER_VIEW vboView;
                vboView.BufferLocation = this->GpuAddress + offsetInBytes;
                vboView.SizeInBytes = sizeInBytes;
                vboView.StrideInBytes = strideInBytes;
                return vboView;
            }

            D3D12_VERTEX_BUFFER_VIEW D3D12GpuBuffer::VertexBufferView(const size_t& baseVertexIndex) const
            {
                const size_t offset = baseVertexIndex * ElementSize;
                return this->VertexBufferView(offset, this->BufferSize - offset, this->ElementSize);
            }

            D3D12_INDEX_BUFFER_VIEW D3D12GpuBuffer::IndexBufferView(const size_t& offsetInBytes, u32 sizeInBytes, bool is32Bit) const
            {
                D3D12_INDEX_BUFFER_VIEW iboView = {}; 
                iboView.BufferLocation = this->GpuAddress + offsetInBytes;
                iboView.SizeInBytes = sizeInBytes;
                iboView.Format = is32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
                return iboView;
            }

            D3D12_INDEX_BUFFER_VIEW D3D12GpuBuffer::IndexBufferView(const size_t& startIndex) const
            {
                const size_t offset = startIndex * ElementSize;
                return this->IndexBufferView(offset, this->BufferSize - offset, this->ElementSize == sizeof(u32));
            }

            D3D12_CPU_DESCRIPTOR_HANDLE D3D12GpuBuffer::ConstantBufferView(u32 offset, u32 size) const
            {
                NT_ASSERT((size_t)(offset + size) <= this->BufferSize, "Trying to create cbo view exceeds size limit of buffer.");
                
                size = Util::alignUp(size, DX_DATA_ALIGN_DEFAULT);
                
                D3D12_CONSTANT_BUFFER_VIEW_DESC cboViewDesc = {};
                cboViewDesc.BufferLocation = this->GpuAddress + offset;
                cboViewDesc.SizeInBytes = size;

                D3D12_CPU_DESCRIPTOR_HANDLE hCbo = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                D3D12Context::g_Device->CreateConstantBufferView(&cboViewDesc, hCbo);
                return hCbo;
            }

            void D3D12GpuBuffer::InitializeDerivedViews(
                TypeGpuBuffer type, 
                /*optional param*/DXGI_FORMAT typedBufferUseOnly,
                /*optional param*/ID3D12Resource* counterRes)
            {                
                D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc    = {};
                srv_desc.ViewDimension                      = D3D12_SRV_DIMENSION_BUFFER;
                srv_desc.Shader4ComponentMapping            = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srv_desc.Buffer.NumElements                 = (type == TypeGpuBuffer::ByteAddressed) ? this->BufferSize / 4 : this->ElementCount;
                
                D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                uav_desc.Buffer.NumElements = (type == TypeGpuBuffer::ByteAddressed) ? this->BufferSize / 4 : this->ElementCount;
                
                if (type == TypeGpuBuffer::Structured)
                {
                    srv_desc.Format = DXGI_FORMAT_UNKNOWN;
                    srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
                    srv_desc.Buffer.StructureByteStride = this->ElementSize;

                    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
                    uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
                    uav_desc.Buffer.StructureByteStride = this->ElementSize;
                    uav_desc.Buffer.CounterOffsetInBytes = 0;
                }
                else
                {
                    srv_desc.Buffer.Flags = (type == TypeGpuBuffer::Typed) ? D3D12_BUFFER_SRV_FLAG_NONE : D3D12_BUFFER_SRV_FLAG_RAW;
                    srv_desc.Format = (type == TypeGpuBuffer::Typed)? typedBufferUseOnly : DXGI_FORMAT_R32_TYPELESS;

                    uav_desc.Buffer.Flags = (type == TypeGpuBuffer::Typed) ? D3D12_BUFFER_UAV_FLAG_NONE : D3D12_BUFFER_UAV_FLAG_RAW;
                    uav_desc.Format = (type == TypeGpuBuffer::Typed) ? typedBufferUseOnly : DXGI_FORMAT_R32_TYPELESS;;
                }

                if (this->SRV.ptr == -1)
                {
                    this->SRV = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
                D3D12Context::g_Device->CreateShaderResourceView(this->pNativeResource, &srv_desc, this->SRV);

                if (this->UAV.ptr == -1)
                {
                    this->UAV = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
                D3D12Context::g_Device->CreateUnorderedAccessView(this->pNativeResource, counterRes, &uav_desc, this->UAV);
            }

            D3D12GpuBuffer::D3D12GpuBuffer()
                : BufferSize(0)
                , ElementCount(0)
                , ElementSize(0)
                , ResourceFlags(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
            {
                this->UAV.ptr = NT_DX_UNKNOWN_ADDRESS;
                this->SRV.ptr = NT_DX_UNKNOWN_ADDRESS;
            }

            D3D12_RESOURCE_DESC D3D12GpuBuffer::DescribeBuffer() const
            {
                D3D12_RESOURCE_DESC desc = {};
                desc.Alignment = 0;
                desc.DepthOrArraySize = 1;
                desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
                desc.Flags = this->ResourceFlags;
                desc.Format = DXGI_FORMAT_UNKNOWN;
                desc.Height = 1;
                desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                desc.MipLevels = 1;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Width = (u64)this->BufferSize;
                return desc;
            }

        }
    }
}
