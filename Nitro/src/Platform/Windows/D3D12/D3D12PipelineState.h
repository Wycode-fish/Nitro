#pragma once

#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class D3D12RootSignature;

			class D3D12PipelineState
			{
			public:
				static std::unordered_map<hashkey_t, ID3D12PipelineState*> g_NativePsoMap_Graphics;
				static std::unordered_map<hashkey_t, ID3D12PipelineState*> g_NativePsoMap_Compute;
				static void DestroyAll();
			public:
				D3D12PipelineState();
				inline void SetRootSignature(const D3D12RootSignature* rs) { m_RootSignature = rs; }
				inline ID3D12PipelineState* GetNativePso() const { return m_NativePso; }
			protected:
				const D3D12RootSignature*		m_RootSignature;
				ID3D12PipelineState*			m_NativePso;
			};


			class D3D12PipelineState_Graphics : public D3D12PipelineState
			{
			public:
				static D3D12_RASTERIZER_DESC	DefaultInit_RasterizerDesc();
				// @ Default blend state: accept pixel shader output
				static D3D12_BLEND_DESC			DefaultInit_BlendDesc();
			public:
				D3D12PipelineState_Graphics();

				// @ Shader Setting
				// @	- byte + size OR d3d12 struct
				inline void SetVertexShader(const void* Binary, size_t Size) { m_PsoDesc.VS = ToD3D12ByteCode((byte*)Binary, Size); }
				inline void SetPixelShader(const void* Binary, size_t Size) { m_PsoDesc.PS = ToD3D12ByteCode((byte*)Binary, Size); }
				inline void SetGeometryShader(const void* Binary, size_t Size) { m_PsoDesc.GS = ToD3D12ByteCode((byte*)Binary, Size); }
				inline void SetHullShader(const void* Binary, size_t Size) { m_PsoDesc.HS = ToD3D12ByteCode((byte*)Binary, Size); }
				inline void SetDomainShader(const void* Binary, size_t Size) { m_PsoDesc.DS = ToD3D12ByteCode((byte*)Binary, Size); }
				
				inline void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) { m_PsoDesc.VS = Binary; }
				inline void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary) { m_PsoDesc.PS = Binary; }
				inline void SetGeometryShader(const D3D12_SHADER_BYTECODE& Binary) { m_PsoDesc.GS = Binary; }
				inline void SetHullShader(const D3D12_SHADER_BYTECODE& Binary) { m_PsoDesc.HS = Binary; }
				inline void SetDomainShader(const D3D12_SHADER_BYTECODE& Binary) { m_PsoDesc.DS = Binary; }

				// @ Other Fields
				void SetPrimitiveTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& tpType);
				void SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, u32 msaaCount, u32 msaaQuality);
				void SetRenderTargetFormats(u32 numOfRtvs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, u32 msaaCount, u32 msaaQuality);
				void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
				void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState);
				void SetBlendState(const D3D12_BLEND_DESC& blenDesc);

				// @ Intro to Sample Mask(MSAA): https://stackoverflow.com/questions/19530924/what-is-glsamplemask
				void SetSampleMask(u32 sampleMask);
				// @ Intro to blend: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml
				void SetInputLayout(u32 numOfElements, D3D12_INPUT_ELEMENT_DESC* elements);

				void Finalize();
				// @ Retrieve native pso's address from global hashmap
				bool TryRetrieveNativePsoAddrFromStorage(hashkey_t keyForThis, /*ref_param*/ID3D12PipelineState**& psRef);

			private:
				inline D3D12_SHADER_BYTECODE ToD3D12ByteCode(const byte* binary, const size_t& size)
				{
					D3D12_SHADER_BYTECODE res = {};
					res.pShaderBytecode = binary;
					res.BytecodeLength = size;
					return res;
				}
				
			private:
				D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PsoDesc;
				std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
			};
		}
	}
}
