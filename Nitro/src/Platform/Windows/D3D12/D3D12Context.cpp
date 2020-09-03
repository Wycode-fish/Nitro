#include "NtPCH.h"
#include "D3D12Context.h"
#include "D3D12SwapChain.h"

// @ ================ Helpers ====================
bool			IsHardwareAdapter1(IDXGIAdapter1* pAdapter)
{
	DXGI_ADAPTER_DESC1 desc;
	pAdapter->GetDesc1(&desc);
	return !(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE);
}
IDXGIAdapter1*	GetAdapter1(IDXGIFactory4* factory4)
{
	IDXGIAdapter1* adapter1 = nullptr;
	u32 adapter_idx = 0;
	while (factory4->EnumAdapters1(adapter_idx, &adapter1) != DXGI_ERROR_NOT_FOUND)
	{
		if (!IsHardwareAdapter1(adapter1))
		{
			adapter_idx++;
			continue;
		}
		HRESULT hr = D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	NT_ASSERT(adapter1, "Adapter creation failed.");
	return adapter1;
}
// @ =============================================

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			IDXGIFactory4*	D3D12Context::g_Factory = nullptr;
			IDXGIAdapter*	D3D12Context::g_Adapter = nullptr;
			ID3D12Device*	D3D12Context::g_Device	= nullptr;

			void D3D12Context::D3D12Initialize()
			{
				HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_Factory));
				NT_ASSERT(SUCCEEDED(hr), "DXGI factory creation failed.");

				g_Adapter = GetAdapter1(g_Factory);

				hr = D3D12CreateDevice(g_Adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_Device));
				NT_ASSERT(SUCCEEDED(hr), "D3D12 device creation failed.");
			}

#pragma region Constructor
			D3D12Context::D3D12Context()
			{
				if (D3D12Context::g_Device == nullptr)
				{
					D3D12Context::D3D12Initialize();
				}
			}
#pragma endregion

#pragma region Destructor
			D3D12Context::~D3D12Context()
			{	
			}
#pragma endregion


#pragma region Control Methods
			void D3D12Context::Init()
			{
			}
			void D3D12Context::SwapBuffers()
			{
				D3D12SwapChain::GetInstance()->Present();
			}
			void D3D12Context::LogCtxSpecs()
			{
			}
#pragma endregion


#pragma region Private Methods

#pragma endregion

			
		}
	}
}

