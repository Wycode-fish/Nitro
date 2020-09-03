#pragma once
#include "D3D12GpuResource.h"

typedef unsigned int u32;

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			struct D3D12ReadbackBuffer : public D3D12GpuBuffer
			{
			public:
				virtual ~D3D12ReadbackBuffer() { this->Destroy(); }
				void	Initialize(u32 numOfElements, u32 elementSize, const std::wstring name = L"");
				void*	Map();
				void	Unmap();
#pragma region Implementation Of Interface Methods
			protected:
				virtual void InitializeDerivedViews() override { /* Do nothing for ReadbackBuffer. */ }
#pragma endregion
			};
		}
	}
}