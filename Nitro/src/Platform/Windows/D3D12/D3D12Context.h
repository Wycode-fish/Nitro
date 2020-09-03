#pragma once

#include "Nitro/Render/IRenderingContext.h"

#define DX_DATA_ALIGN_DEFAULT 16
#define DX_ALIGN __declspec(align(DX_DATA_ALIGN_DEFAULT))

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class D3D12SwapChain;

			class D3D12Context : public IRenderingContext
			{
			public:
				static IDXGIFactory4*	g_Factory;
				static IDXGIAdapter*	g_Adapter;
				static ID3D12Device*	g_Device;
				static void D3D12Initialize();

			public:
				D3D12Context();
				virtual ~D3D12Context();
			public:
				virtual void Init() override;
				virtual void SwapBuffers() override;

			public:			
			protected:
				virtual void LogCtxSpecs() override;

			private:
			};
		}
	
	}
}