#include "NtPCH.h"
#include "D3D12GpuResource.h"

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

        }
    }
}
