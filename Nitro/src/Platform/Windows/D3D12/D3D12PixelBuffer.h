#pragma once
#include "D3D12GpuResource.h"
#include "D3D12Color.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
#pragma region Forward Declaration
			class D3D12CommandContext;
#pragma endregion

			// @ -----------------------------------------------------------------------------
			// @								PixelBuffer
			// @ -----------------------------------------------------------------------------
			struct D3D12PixelBuffer : public D3D12GpuResource
			{
				// @ both width / height / ArraySize are numbers in pixels
				u32			Width;
				u32			Height;
				u32			ArraySize;
				DXGI_FORMAT Format;

				D3D12PixelBuffer();
				void ExportToFile(const std::string& path);

#pragma region Static Methods
				static size_t		BytesPerPixel(DXGI_FORMAT Format);

				static DXGI_FORMAT	ClassifyUAVFormat(DXGI_FORMAT specificFormat);
				static DXGI_FORMAT	ClassifyDSVFormat(DXGI_FORMAT specificFormat);
				// ?? what's diff between ClassifyDSV & ClassifyDepth
				static DXGI_FORMAT	ClassifyDepthFormat(DXGI_FORMAT specificFormat); 
				static DXGI_FORMAT	ClassifyStencilFormat(DXGI_FORMAT specificFormat);
#pragma endregion

			protected:
				D3D12_RESOURCE_DESC ResetAndDescribeTex2D(
					u32 width, u32 height, u32 depthOrArraySize, 
					u32 numMipMap, DXGI_FORMAT format, u32 flags);

				void InitializeAsTextureResource(
					ID3D12Device* device, 
					const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE clearVal, 
					D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = -1/*an initial value totally of no use...*/, 
					const std::wstring name = L"");

				void AssociateToResource(
					ID3D12Device* device, 
					ID3D12Resource* nativeRes, D3D12_RESOURCE_STATES currentState, 
					const std::wstring name = L"");
			};

			// @ -----------------------------------------------------------------------------
			// @								ColorBuffer
			// @ -----------------------------------------------------------------------------
			struct D3D12ColorBuffer : public D3D12PixelBuffer
			{
#pragma region Member Variables
				Color						ClearColor;
				D3D12_CPU_DESCRIPTOR_HANDLE SRV;
				D3D12_CPU_DESCRIPTOR_HANDLE RTV;
				D3D12_CPU_DESCRIPTOR_HANDLE UAVs[12];	// Each mip-level of texture has a UAV
				u32							NumOfMipMaps;
				// For MSAA purpose
				u32							NumOfFragments;
				u32							NumOfSamples;
#pragma endregion

#pragma region Static Methods
			public:
				// @ NumOfMips is always 1 larger then NumOfMipMaps, they're different
				// @		- e.g.	A 2x2 image, NumOfMips is 2, NumOfMipMaps is 1.
				// @				A 1x1 image, NumOfMips is 1, NumOfMipMaps is 0.
				static u32	ComputeNumMips(u32 width, u32 height);
				static void FillDerivedViewDescs_Default(D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc, D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, u32 arraySize, u32 numOfMips, DXGI_FORMAT format, bool isMsaaEnabled = false);
#pragma endregion

#pragma region Control Methods
			public:
				D3D12ColorBuffer(Color clearColor = Color(0.0f, 0.0f, 0.0f, 0.0f));
				void Initialize(u32 width, u32 height, u32 numOfMips, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS vidmem = -1, const std::wstring name = L"");
				void InitializeFromSwapChain(ID3D12Resource* baseResource, const std::wstring name = L"");
				void GenerateMipMaps(D3D12CommandContext& ctx);
#pragma endregion

			protected:
				D3D12_RESOURCE_FLAGS	ResourceFlags() const;
				void					InitializeDerivedViews(ID3D12Device* device, DXGI_FORMAT format, u32 arraySize, u32 numOfMips = 1);
			};

			// @ -----------------------------------------------------------------------------
			// @								DepthBuffer
			// @ -----------------------------------------------------------------------------
			struct D3D12DepthBuffer : public D3D12PixelBuffer
			{
				// @ DSV Types
				enum : u8
				{
					DSV_ReadWrite = 0,
					DSV_DepthReadOnly = 1,
					DSV_StencilReadOnly,
					DSV_ReadOnly
				};
#pragma region Member Variables
				D3D12_CPU_DESCRIPTOR_HANDLE DSVs[4];
				D3D12_CPU_DESCRIPTOR_HANDLE SRV_Depth;
				D3D12_CPU_DESCRIPTOR_HANDLE SRV_Stencil;
				float	ClearDepth;
				u8		ClearStencil;
#pragma endregion

				D3D12DepthBuffer(float clearDepth = 0.0f, u8 clearStencil = 0);
				// Create a depth buffer.  If an address is supplied, memory will not be allocated.
				// The vmem address allows you to alias buffers (which can be especially useful for
				// reusing ESRAM across a frame.)
				void Initialize(u32 width, u32 height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS vidmem = -1, const std::wstring& name = L"");

				void Initialize(uint32_t Width, uint32_t Height, uint32_t NumSamples, DXGI_FORMAT Format,
					D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr = -1, const std::wstring& Name = L"");

				inline const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV(u8 accessibility) { return this->DSVs[accessibility]; }

			protected:
				void InitializeDerivedViews(ID3D12Device* device, DXGI_FORMAT format);
			};
		}
	}
}