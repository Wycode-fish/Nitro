#pragma once

#include "Nitro/Render/IRenderingContext.h"

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

				// @ ----------------------------
				// @		Eason's attempt
				// @ ----------------------------
				virtual void Clear() const override;
			public:			
			protected:
				virtual void LogCtxSpecs() override;

			private:
			};
		}
	
	}
}