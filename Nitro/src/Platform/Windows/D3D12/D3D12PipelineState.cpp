#include "NtPCH.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12SwapChain.h"
#include "D3D12Context.h"

#include "Nitro/Util/HashOps.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			std::unordered_map<hashkey_t, ID3D12PipelineState*> D3D12PipelineState::g_NativePsoMap_Graphics;
			std::unordered_map<hashkey_t, ID3D12PipelineState*> D3D12PipelineState::g_NativePsoMap_Compute;
			
			void D3D12PipelineState::DestroyAll()
			{
				for (auto nativePSO : g_NativePsoMap_Graphics)
				{
					NT_DX_RELEASE(nativePSO.second);
				}
				for (auto nativePSO : g_NativePsoMap_Compute)
				{
					NT_DX_RELEASE(nativePSO.second);
				}
				g_NativePsoMap_Graphics.clear();
				g_NativePsoMap_Compute.clear();
			}

			D3D12PipelineState::D3D12PipelineState()
				: m_RootSignature(nullptr)
			{
			}
			D3D12_RASTERIZER_DESC D3D12PipelineState_Graphics::DefaultInit_RasterizerDesc()
			{
				D3D12_RASTERIZER_DESC rd = {};
				rd.FillMode = D3D12_FILL_MODE_SOLID;
				rd.CullMode = D3D12_CULL_MODE_NONE;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
				rd.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
				rd.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
				rd.DepthClipEnable = TRUE;
				rd.MultisampleEnable = FALSE;
				rd.AntialiasedLineEnable = FALSE;
				rd.ForcedSampleCount = 0;
				rd.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

				return rd;
			}
			D3D12_BLEND_DESC D3D12PipelineState_Graphics::DefaultInit_BlendDesc()
			{
				D3D12_BLEND_DESC bd = {};
				bd.AlphaToCoverageEnable = false;
				bd.IndependentBlendEnable = false;
				D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {};
				defaultRenderTargetBlendDesc.BlendEnable = false;
				defaultRenderTargetBlendDesc.LogicOpEnable = false;
				defaultRenderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
				defaultRenderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
				defaultRenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				defaultRenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
				defaultRenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				defaultRenderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
				defaultRenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				{
					bd.RenderTarget[i] = defaultRenderTargetBlendDesc;
				}
				return bd;
			}
			D3D12PipelineState_Graphics::D3D12PipelineState_Graphics()
			{
				memset(&m_PsoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
				m_PsoDesc.NodeMask = 0;
				m_PsoDesc.SampleMask = 0xFFFFFFFFu;
				m_PsoDesc.SampleDesc.Count = 1;
				m_PsoDesc.InputLayout.NumElements = 0;
				m_PsoDesc.NumRenderTargets = D3D12SwapChain::GetInstance()->GetFrameBufferCount();
			}

			void D3D12PipelineState_Graphics::SetPrimitiveTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& tpType) 
			{ 
				NT_ASSERT(tpType != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, "Can't draw with topology type: undefined."); 
				m_PsoDesc.PrimitiveTopologyType = tpType; 
			}

			void D3D12PipelineState_Graphics::SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, u32 msaaCount, u32 msaaQuality)
			{
				this->SetRenderTargetFormats(1, &rtvFormat, dsvFormat, msaaCount, msaaQuality);
			}

			void D3D12PipelineState_Graphics::SetRenderTargetFormats(u32 numOfRtvs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, u32 msaaCount, u32 msaaQuality)
			{
				NT_ASSERT(numOfRtvs == 0 || rtvFormats != nullptr, "Input param: numOfRtvs / rtvFormats doesn't match");
				for (u32 i = 0; i < m_PsoDesc.NumRenderTargets; ++i)
				{
					if (i < numOfRtvs)
					{
						m_PsoDesc.RTVFormats[i] = rtvFormats[i];
					}
					else
					{
						m_PsoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
					}
				}
				m_PsoDesc.NumRenderTargets		= numOfRtvs;
				m_PsoDesc.DSVFormat				= dsvFormat;
				m_PsoDesc.SampleDesc.Count		= msaaCount;
				m_PsoDesc.SampleDesc.Quality	= msaaQuality;
			}

			void D3D12PipelineState_Graphics::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc)
			{
				m_PsoDesc.RasterizerState = rasterizerDesc;
			}

			void D3D12PipelineState_Graphics::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState)
			{
				m_PsoDesc.DepthStencilState = depthStencilState;
			}

			void D3D12PipelineState_Graphics::SetBlendState(const D3D12_BLEND_DESC& blenDesc)
			{
				m_PsoDesc.BlendState = blenDesc;
			}

			void D3D12PipelineState_Graphics::SetSampleMask(u32 sampleMask)
			{
				m_PsoDesc.SampleMask = sampleMask;
			}

			void D3D12PipelineState_Graphics::SetInputLayout(u32 numOfElements, D3D12_INPUT_ELEMENT_DESC* elements)
			{
				m_PsoDesc.InputLayout.NumElements = numOfElements;
				if (numOfElements > 0)
				{
					D3D12_INPUT_ELEMENT_DESC* newElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * numOfElements);
					memcpy(newElements, elements, numOfElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
					m_InputLayout.reset((const D3D12_INPUT_ELEMENT_DESC*)newElements);
				}
				else
				{
					m_InputLayout = nullptr;
				}
			}

			void D3D12PipelineState_Graphics::Finalize()
			{
				m_PsoDesc.pRootSignature = const_cast<ID3D12RootSignature*>(m_RootSignature->GetNativeRootSignature());
				NT_ASSERT(m_PsoDesc.pRootSignature != nullptr, "root signature is null");

#pragma region Hash Key Retrieval
				// @ Empty pInputElementDescs for deduction of hash key of object.
				m_PsoDesc.InputLayout.pInputElementDescs = nullptr;

				hashkey_t hk = Nitro::Util::hash_state(&m_PsoDesc);
				hk = Nitro::Util::hash_state(m_InputLayout.get(), m_PsoDesc.InputLayout.NumElements, hk);

				// @ Assign pInputElementDescs after deduction of hash key of object.
				m_PsoDesc.InputLayout.pInputElementDescs = m_InputLayout.get();
#pragma endregion

				ID3D12PipelineState** psRef;
				bool alreadyInMap = this->TryRetrieveNativePsoAddrFromStorage(hk, /*out_param*/psRef);

				if (alreadyInMap)
				{
					while (*psRef == nullptr)
					{
						std::this_thread::yield();
					}
				}
				else
				{
					HRESULT hr = D3D12Context::g_Device->CreateGraphicsPipelineState(&m_PsoDesc, IID_PPV_ARGS(psRef));
					NT_ASSERT(SUCCEEDED(hr), "pso created failed. Error: {0}", Util::integral_to_hexstr(hr));
				}

				m_NativePso = *psRef;
				D3D12PipelineState::g_NativePsoMap_Graphics[hk] = m_NativePso;
			}

			bool D3D12PipelineState_Graphics::TryRetrieveNativePsoAddrFromStorage(hashkey_t keyForThis, ID3D12PipelineState**& psRef)
			{
				static std::mutex mutex_map;
				std::lock_guard<std::mutex> internal_lock(mutex_map);

				bool alreadyInMap = false;
				// std::unordered_map<hashkey_t, ID3D12PipelineState*>::const_iterator 
				auto it = D3D12PipelineState::g_NativePsoMap_Graphics.find(keyForThis);
				if (it != D3D12PipelineState::g_NativePsoMap_Graphics.end())
				{
					alreadyInMap = true;
				}
				psRef = &D3D12PipelineState::g_NativePsoMap_Graphics[keyForThis];
				return alreadyInMap;
			}
		}
	}
}
