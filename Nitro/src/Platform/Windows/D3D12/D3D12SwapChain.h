#pragma once

class IDXGISwapChain;
class ID3D12Resource;

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class D3D12CommandQueue;
			class D3D12ColorBuffer;

			class D3D12SwapChain
			{
			private:
				static D3D12SwapChain* m_Instance;
			public:
				static D3D12SwapChain* GetInstance();
				NON_COPYABLE(D3D12SwapChain);

			public:
				// @ Swap chain creation needs a command queue attached in previous implementation
				// @ Check D3D12Mess.sln
				void	Init();
				void	ShutDown();
				
				// TODO: not sure what to do here...
				void	SwapBuffer();
				
				// TODO: Temp API for d3d12swapchain functions
				void	Present();
				bool	IsFullScreenState();
				void	SetFullScreenState(bool isToFullScreen);

				inline D3D12ColorBuffer&				GetCurrentFrameBufferRef() { return *this->m_Buffers[this->GetCurrentFrameBufferIndex()]; }
				inline D3D12ColorBuffer*				GetCurrentFrameBuffer() { return m_Buffers[this->GetCurrentFrameBufferIndex()]; }
				inline u32								GetCurrentFrameBufferIndex() const { IDXGISwapChain3* swap_chain = static_cast<IDXGISwapChain3*>(m_NativeChain); return swap_chain->GetCurrentBackBufferIndex(); }
				inline u32								GetFrameBufferCount() const { return m_Buffers.size(); }
				inline std::vector<D3D12ColorBuffer*>&	GetFrameBuffers() { return m_Buffers; }

			private:
				D3D12SwapChain(const size_t& numOfBuffers = NT_DX_SWAPCHAIN_FRAME_BUFFER_NUM);

				void FillBufferDesc(DXGI_MODE_DESC& bufferDesc) const;
				void FillSampleDesc(DXGI_SAMPLE_DESC& sampleDesc) const;
				void FillSwapChainDesc(DXGI_SWAP_CHAIN_DESC& scDesc) const;

				void RetrieveBuffers();
			private:
				IDXGISwapChain* m_NativeChain;
				// @ TODO: use nitro-wrapper of id3d12resource.
				std::vector<D3D12ColorBuffer*> m_Buffers;
			};
		}
	}
}