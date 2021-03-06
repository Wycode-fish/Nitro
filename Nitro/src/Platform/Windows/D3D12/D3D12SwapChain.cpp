#include "NtPCH.h"
#include "D3D12SwapChain.h"
#include "D3D12Context.h"
#include "Nitro/Application.h"
#include "D3D12CommandQueue.h"
#include "D3D12PixelBuffer.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			D3D12SwapChain* D3D12SwapChain::m_Instance = nullptr;
			D3D12SwapChain* D3D12SwapChain::GetInstance()
			{
				if (m_Instance == nullptr)
				{
					m_Instance = nitro_new D3D12SwapChain();
				}
				return m_Instance;
			}

			D3D12SwapChain::D3D12SwapChain(const size_t& numOfBuffers)
				: m_NativeChain(nullptr)
				, m_Buffers(numOfBuffers, nullptr)
			{
				Init();
			}

			void D3D12SwapChain::Init()
			{
				if (m_NativeChain != nullptr) return;

				DXGI_SWAP_CHAIN_DESC scDesc = {};
				this->FillSwapChainDesc(scDesc);

				HRESULT hr = D3D12Context::g_Factory->CreateSwapChain(
					D3D12CommandQueueManager::GetInstance()->GetCommandQueueByType(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetNativeCommandQueue(), 
					&scDesc, 
					&m_NativeChain
				);
				NT_ASSERT(SUCCEEDED(hr), "Swap chain creation failed. Error: 0x{0}", Nitro::Util::integral_to_hexstr(hr));
				
				this->RetrieveBuffers();
			}

			void D3D12SwapChain::ShutDown()
			{
				for (u32 i = 0; i < this->m_Buffers.size(); ++i)
				{
					if (this->m_Buffers[i])
					{
						delete this->m_Buffers[i];
					}
				}
				NT_DX_RELEASE(this->m_NativeChain);
			}

			void D3D12SwapChain::SwapBuffer()
			{
				// @ TODO: do swap buffer here maybe?
			}

			void D3D12SwapChain::Present()
			{
				HRESULT hr = m_NativeChain->Present(0, 0);
				NT_ASSERT(SUCCEEDED(hr), "Swap chain present failed. Error: 0x{0}", Nitro::Util::integral_to_hexstr(hr));
			}

			bool D3D12SwapChain::IsFullScreenState()
			{
				int isFullScreen;
				m_NativeChain->GetFullscreenState(&isFullScreen, nullptr);
				return isFullScreen > 0 ? true : false;
			}

			void D3D12SwapChain::SetFullScreenState(bool isToFullScreen)
			{
				if (this->IsFullScreenState() != isToFullScreen)
				{
					m_NativeChain->SetFullscreenState(isToFullScreen, nullptr);
				}
			}

			void D3D12SwapChain::FillBufferDesc(DXGI_MODE_DESC& bufferDesc) const
			{
				std::unique_ptr<Nitro::Base::Window>& window = Nitro::Base::Application::GetInstance()->GetWindow();
				bufferDesc.Width = window->GetWidth();
				bufferDesc.Height = window->GetHeight();
				// @ Valid format for display: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173064(v=vs.85)
				bufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
			}

			void D3D12SwapChain::FillSwapChainDesc(DXGI_SWAP_CHAIN_DESC& scDesc) const
			{
				std::unique_ptr<Nitro::Base::Window>& window = Nitro::Base::Application::GetInstance()->GetWindow();

				scDesc.BufferCount = m_Buffers.size();
				scDesc.OutputWindow = (HWND)window->GetNativeWindow();
				scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				scDesc.Windowed = true;	// @ full screen setting to be done later.
				scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
				scDesc.SampleDesc.Count = 1;
				scDesc.SampleDesc.Quality = 0;
				scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

				DXGI_MODE_DESC bufferDesc = {};
				this->FillBufferDesc(bufferDesc);
				scDesc.BufferDesc = bufferDesc;
			}

			void D3D12SwapChain::RetrieveBuffers()
			{
				for (u32 i = 0; i < m_Buffers.size(); ++i)
				{
					m_Buffers[i] = nitro_new D3D12ColorBuffer();

					ID3D12Resource* nativeBuffer = nullptr;
					m_NativeChain->GetBuffer(i, IID_PPV_ARGS(&nativeBuffer));
					std::wstring fboName(L"FB-");
#ifdef NT_DEBUG
					fboName += std::to_wstring(i);
#endif
					m_Buffers[i]->InitializeFromSwapChain(nativeBuffer, fboName);
				}
			}
		}
	}
}

