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
			// @							GpuBuffer
			// @ -------------------------------------------------------------------
			struct D3D12GpuBuffer : public D3D12GpuResource
			{

			};
		}
	}
}