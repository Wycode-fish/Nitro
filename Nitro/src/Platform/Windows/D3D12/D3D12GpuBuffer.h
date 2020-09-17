#pragma once

#include "D3D12GpuResource.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
#pragma region Forward Declaration
			class D3D12CommandContext;
#pragma endregion
			/* ------------------------------------------
				GpuBuffer:
					- ElementCount
					- ElementSize
					- ResourceFlags
					- UAV
					- SRV

				* Derived Class:
					- ByteAddressed Buffer
					- Structured    Buffer
					- Typed		    Buffer
			--------------------------------------------- */

			// @ -------------------------------------------------------
			// @				ByteAddressedBuffer
			// @ * Each element is of DWORD size.
			// @ -------------------------------------------------------
			struct D3D12ByteAddressedBuffer : public D3D12GpuBuffer
			{
				virtual void InitializeDerivedViews() override;
			};

			// @ -------------------------------------------------------
			// @				StructuredBuffer
			// @ * Associate to a counter buffer inside
			// @ * UAV view description diff:
			// @			- format
			// @			- buffer.stride + buffer.counterOffset
			// @ -------------------------------------------------------
			struct D3D12StructuredBuffer : public D3D12GpuBuffer
			{
				D3D12ByteAddressedBuffer m_CounterBuffer;

				virtual void Destroy() override;
				virtual void InitializeDerivedViews() override;
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetCounterSRV(D3D12CommandContext& ctx);
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetCounterUAV(D3D12CommandContext& ctx);
			};

			// @ -------------------------------------------------------
			// @				TypedBuffer
			// @ * Predefined format of elements in buffer
			// @ * view description diff:
			// @			- format
			// @ -------------------------------------------------------
			struct D3D12TypedBuffer : public D3D12GpuBuffer
			{
				DXGI_FORMAT Format;

				D3D12TypedBuffer(DXGI_FORMAT format);
				virtual void InitializeDerivedViews() override;
			};
		}
	}
}