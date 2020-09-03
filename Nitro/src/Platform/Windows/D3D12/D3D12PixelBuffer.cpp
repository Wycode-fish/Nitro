#include "NtPCH.h"
#include "D3D12PixelBuffer.h"
#include "D3D12ReadbackBuffer.h"
#include "D3D12Context.h"
#include "D3D12CommandContext.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
            size_t D3D12PixelBuffer::BytesPerPixel(DXGI_FORMAT Format)
            {
                switch (Format)
                {
                case DXGI_FORMAT_R32G32B32A32_TYPELESS:
                case DXGI_FORMAT_R32G32B32A32_FLOAT:
                case DXGI_FORMAT_R32G32B32A32_UINT:
                case DXGI_FORMAT_R32G32B32A32_SINT:
                    return 16;

                case DXGI_FORMAT_R32G32B32_TYPELESS:
                case DXGI_FORMAT_R32G32B32_FLOAT:
                case DXGI_FORMAT_R32G32B32_UINT:
                case DXGI_FORMAT_R32G32B32_SINT:
                    return 12;

                case DXGI_FORMAT_R16G16B16A16_TYPELESS:
                case DXGI_FORMAT_R16G16B16A16_FLOAT:
                case DXGI_FORMAT_R16G16B16A16_UNORM:
                case DXGI_FORMAT_R16G16B16A16_UINT:
                case DXGI_FORMAT_R16G16B16A16_SNORM:
                case DXGI_FORMAT_R16G16B16A16_SINT:
                case DXGI_FORMAT_R32G32_TYPELESS:
                case DXGI_FORMAT_R32G32_FLOAT:
                case DXGI_FORMAT_R32G32_UINT:
                case DXGI_FORMAT_R32G32_SINT:
                case DXGI_FORMAT_R32G8X24_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                    return 8;

                case DXGI_FORMAT_R10G10B10A2_TYPELESS:
                case DXGI_FORMAT_R10G10B10A2_UNORM:
                case DXGI_FORMAT_R10G10B10A2_UINT:
                case DXGI_FORMAT_R11G11B10_FLOAT:
                case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                case DXGI_FORMAT_R8G8B8A8_UNORM:
                case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                case DXGI_FORMAT_R8G8B8A8_UINT:
                case DXGI_FORMAT_R8G8B8A8_SNORM:
                case DXGI_FORMAT_R8G8B8A8_SINT:
                case DXGI_FORMAT_R16G16_TYPELESS:
                case DXGI_FORMAT_R16G16_FLOAT:
                case DXGI_FORMAT_R16G16_UNORM:
                case DXGI_FORMAT_R16G16_UINT:
                case DXGI_FORMAT_R16G16_SNORM:
                case DXGI_FORMAT_R16G16_SINT:
                case DXGI_FORMAT_R32_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT:
                case DXGI_FORMAT_R32_FLOAT:
                case DXGI_FORMAT_R32_UINT:
                case DXGI_FORMAT_R32_SINT:
                case DXGI_FORMAT_R24G8_TYPELESS:
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
                case DXGI_FORMAT_R8G8_B8G8_UNORM:
                case DXGI_FORMAT_G8R8_G8B8_UNORM:
                case DXGI_FORMAT_B8G8R8A8_UNORM:
                case DXGI_FORMAT_B8G8R8X8_UNORM:
                case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
                case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                case DXGI_FORMAT_B8G8R8X8_TYPELESS:
                case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    return 4;

                case DXGI_FORMAT_R8G8_TYPELESS:
                case DXGI_FORMAT_R8G8_UNORM:
                case DXGI_FORMAT_R8G8_UINT:
                case DXGI_FORMAT_R8G8_SNORM:
                case DXGI_FORMAT_R8G8_SINT:
                case DXGI_FORMAT_R16_TYPELESS:
                case DXGI_FORMAT_R16_FLOAT:
                case DXGI_FORMAT_D16_UNORM:
                case DXGI_FORMAT_R16_UNORM:
                case DXGI_FORMAT_R16_UINT:
                case DXGI_FORMAT_R16_SNORM:
                case DXGI_FORMAT_R16_SINT:
                case DXGI_FORMAT_B5G6R5_UNORM:
                case DXGI_FORMAT_B5G5R5A1_UNORM:
                case DXGI_FORMAT_A8P8:
                case DXGI_FORMAT_B4G4R4A4_UNORM:
                    return 2;

                case DXGI_FORMAT_R8_TYPELESS:
                case DXGI_FORMAT_R8_UNORM:
                case DXGI_FORMAT_R8_UINT:
                case DXGI_FORMAT_R8_SNORM:
                case DXGI_FORMAT_R8_SINT:
                case DXGI_FORMAT_A8_UNORM:
                case DXGI_FORMAT_P8:
                    return 1;

                default:
                    return 0;
                }
            }

            DXGI_FORMAT D3D12PixelBuffer::ClassifyUAVFormat(DXGI_FORMAT specificFormat)
            {
                switch (specificFormat)
                {
                case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                case DXGI_FORMAT_R8G8B8A8_UNORM:
                case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    return DXGI_FORMAT_R8G8B8A8_UNORM;

                case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                case DXGI_FORMAT_B8G8R8A8_UNORM:
                case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    return DXGI_FORMAT_B8G8R8A8_UNORM;

                case DXGI_FORMAT_B8G8R8X8_TYPELESS:
                case DXGI_FORMAT_B8G8R8X8_UNORM:
                case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    return DXGI_FORMAT_B8G8R8X8_UNORM;

                case DXGI_FORMAT_R32_TYPELESS:
                case DXGI_FORMAT_R32_FLOAT:
                    return DXGI_FORMAT_R32_FLOAT;

#ifdef NT_DEBUG
                case DXGI_FORMAT_R32G8X24_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                case DXGI_FORMAT_D32_FLOAT:
                case DXGI_FORMAT_R24G8_TYPELESS:
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                case DXGI_FORMAT_D16_UNORM:

                    NT_ASSERT(false, "Requested a UAV format for a depth stencil format.");
#endif

                default:
                    return specificFormat;
                }
            }

            DXGI_FORMAT D3D12PixelBuffer::ClassifyDSVFormat(DXGI_FORMAT specificFormat)
            {
                switch (specificFormat)
                {
                    // 32-bit Z w/ Stencil
                case DXGI_FORMAT_R32G8X24_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                    return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

                    // No Stencil
                case DXGI_FORMAT_R32_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT:
                case DXGI_FORMAT_R32_FLOAT:
                    return DXGI_FORMAT_D32_FLOAT;

                    // 24-bit Z
                case DXGI_FORMAT_R24G8_TYPELESS:
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                    return DXGI_FORMAT_D24_UNORM_S8_UINT;

                    // 16-bit Z w/o Stencil
                case DXGI_FORMAT_R16_TYPELESS:
                case DXGI_FORMAT_D16_UNORM:
                case DXGI_FORMAT_R16_UNORM:
                    return DXGI_FORMAT_D16_UNORM;

                default:
                    return specificFormat;
                }
            }

            DXGI_FORMAT D3D12PixelBuffer::ClassifyDepthFormat(DXGI_FORMAT specificFormat)
            {
                switch (specificFormat)
                {
                    // 32-bit Z w/ Stencil
                case DXGI_FORMAT_R32G8X24_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                    return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

                    // No Stencil
                case DXGI_FORMAT_R32_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT:
                case DXGI_FORMAT_R32_FLOAT:
                    return DXGI_FORMAT_R32_FLOAT;

                    // 24-bit Z
                case DXGI_FORMAT_R24G8_TYPELESS:
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

                    // 16-bit Z w/o Stencil
                case DXGI_FORMAT_R16_TYPELESS:
                case DXGI_FORMAT_D16_UNORM:
                case DXGI_FORMAT_R16_UNORM:
                    return DXGI_FORMAT_R16_UNORM;

                default:
                    return DXGI_FORMAT_UNKNOWN;
                }
            }

            DXGI_FORMAT D3D12PixelBuffer::ClassifyStencilFormat(DXGI_FORMAT specificFormat)
            {
                switch (specificFormat)
                {
                    // 32-bit Z w/ Stencil
                case DXGI_FORMAT_R32G8X24_TYPELESS:
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                    return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

                    // 24-bit Z
                case DXGI_FORMAT_R24G8_TYPELESS:
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                    return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

                default:
                    return DXGI_FORMAT_UNKNOWN;
                }
            }

            D3D12_RESOURCE_DESC D3D12PixelBuffer::ResetAndDescribeTex2D(u32 width, u32 height, u32 depthOrArraySize, u32 numMipMap, DXGI_FORMAT format, u32 flags)
            {
                this->Width     = width;
                this->Height    = height;
                this->ArraySize = depthOrArraySize;
                this->Format    = format;

                D3D12_RESOURCE_DESC desc = {};
                desc.Format             = format;
                desc.Flags              = (D3D12_RESOURCE_FLAGS)flags;
                desc.Width              = width;
                desc.Height             = height;
                desc.DepthOrArraySize   = depthOrArraySize;
                desc.MipLevels          = numMipMap;
                desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                desc.Alignment          = 0;
                desc.SampleDesc.Count   = 1;
                desc.SampleDesc.Quality = 0;

                return desc;
            }

            void D3D12PixelBuffer::InitializeAsTextureResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE clearVal, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr/*actually no use*/, const std::wstring name)
            {
                this->Destroy();

                Util::DX::HEAP_PROPS props(D3D12_HEAP_TYPE_DEFAULT);
                HRESULT hr = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, &clearVal, IID_PPV_ARGS(&this->pNativeResource));
                NT_ASSERT(SUCCEEDED(hr), "Texture creation failed.");

                this->UsageState = D3D12_RESOURCE_STATE_COMMON;
                this->GpuAddress = 0; // ?? Why not this->pNativeResource->GetGPUVirtualAddress()?
#ifdef NT_DEBUG
                this->pNativeResource->SetName(name.c_str());
#endif // NT_DEBUG
            }

            void D3D12PixelBuffer::AssociateToResource(ID3D12Device* device, ID3D12Resource* nativeRes, D3D12_RESOURCE_STATES currentState, const std::wstring name)
            {
                NT_ASSERT(nativeRes != nullptr, "Can't associate this to a null resource.");
                D3D12_RESOURCE_DESC resDesc = nativeRes->GetDesc();

                // @ Release original resource before associate to another
                NT_DX_RELEASE(this->pNativeResource);

                this->pNativeResource   = nativeRes;
                this->UsageState        = currentState;
                this->Width             = resDesc.Width;
                this->Height            = resDesc.Height;
                this->ArraySize         = resDesc.DepthOrArraySize;
                this->Format            = resDesc.Format;
#ifdef NT_DEBUG
                this->pNativeResource->SetName(name.c_str());
#endif
            }
			
            D3D12PixelBuffer::D3D12PixelBuffer()
				: Width(0), Height(0), ArraySize(0), Format(DXGI_FORMAT_UNKNOWN)
			{}

			void D3D12PixelBuffer::ExportToFile(const std::string& path)
			{
				// @ For a PixelBuffer, each element within is a Pixel.
				D3D12ReadbackBuffer readbackBuffer;
				readbackBuffer.Initialize(this->Width * this->Height, D3D12PixelBuffer::BytesPerPixel(this->Format));

                // @ PixelBuffer itself is not Cpu-Accessible(no Map()), hence copy to a ReadbackBuffer and do Map()
                D3D12CommandContext::ReadbackTexture2D(readbackBuffer, *this);
                void* cpuAddr = readbackBuffer.Map();

                std::ofstream outFile(path, std::ios::out | std::ios::binary);
                outFile.write((const char*)&this->Format, 4);
                outFile.write((const char*)&this->Width, 4);    // pitch
                outFile.write((const char*)&this->Width, 4);
                outFile.write((const char*)&this->Height, 4);
                outFile.write((const char*)cpuAddr, readbackBuffer.BufferSize);
                outFile.close();

                readbackBuffer.Unmap();
			}

            u32 D3D12ColorBuffer::ComputeNumMips(u32 width, u32 height)
            {
                u32 highestBitIndex;
                _BitScanReverse((unsigned long*)&highestBitIndex, width | height);
                return highestBitIndex + 1;
            }

            void D3D12ColorBuffer::FillDerivedViewDescs_Default(
                D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc, D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, 
                u32 arraySize, u32 numOfMips, DXGI_FORMAT format, bool isMsaaEnabled)
            {
                NT_ASSERT(arraySize == 1 || numOfMips == 1, "We don't support auto-mips on texture arrays.");

                rtvDesc.Format = format;
                srvDesc.Format = format;
                uavDesc.Format = D3D12PixelBuffer::ClassifyUAVFormat(format);

                // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_shader_resource_view_desc
                //      * The default 1:1 mapping can be indicated by specifying D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

                if (arraySize > 1)  // If we use texture2D array, we gave up mip-map functionality
                {
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;

                    rtvDesc.Texture2DArray.MipSlice = 0;
                    rtvDesc.Texture2DArray.FirstArraySlice = 0;
                    rtvDesc.Texture2DArray.ArraySize = arraySize;

                    uavDesc.Texture2DArray.MipSlice = 0;
                    uavDesc.Texture2DArray.FirstArraySlice = 0;
                    uavDesc.Texture2DArray.ArraySize = arraySize;

                    srvDesc.Texture2DArray.MipLevels = 0;
                    srvDesc.Texture2DArray.FirstArraySlice = 0;
                    srvDesc.Texture2DArray.MostDetailedMip = 0;
                    srvDesc.Texture2DArray.ArraySize = arraySize;
                }
                else // Only when mip-map is enabled, we considered MSAA;
                {
                    if (isMsaaEnabled)
                    {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
                        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

                        rtvDesc.Texture2D.MipSlice = 0;
                        uavDesc.Texture2D.MipSlice = 0;
                        srvDesc.Texture2D.MipLevels = numOfMips;

                        srvDesc.Texture2D.MostDetailedMip = 0;
                    }
                }
            }

            D3D12ColorBuffer::D3D12ColorBuffer(Color clearColor)
                : ClearColor(clearColor), NumOfMipMaps(0), NumOfFragments(1), NumOfSamples(1)
            {
                this->RTV.ptr = -1;
                this->SRV.ptr = -1;
                memset(this->UAVs, 0xFF, sizeof(this->UAVs));
            }

            void D3D12ColorBuffer::Initialize(u32 width, u32 height, u32 numOfMips, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS vidmem, const std::wstring name)
            {
                numOfMips = numOfMips == 0 ? this->ComputeNumMips (width, height) : numOfMips;

                D3D12_RESOURCE_FLAGS initFlags = this->ResourceFlags();
                
                D3D12_RESOURCE_DESC desc = this->ResetAndDescribeTex2D(width, height, 1, numOfMips, format, initFlags);
                desc.SampleDesc.Count = this->NumOfFragments;
                desc.SampleDesc.Quality = 0;

                D3D12_CLEAR_VALUE clearVal = {};
                clearVal.Format = format;
                clearVal.Color[0] = this->ClearColor.R();
                clearVal.Color[1] = this->ClearColor.G();
                clearVal.Color[2] = this->ClearColor.B();
                clearVal.Color[3] = this->ClearColor.A();

                this->InitializeAsTextureResource(D3D12Context::g_Device, desc, clearVal, vidmem, name);
                this->InitializeDerivedViews(D3D12Context::g_Device, format, 1, numOfMips);
            }

            void D3D12ColorBuffer::InitializeFromSwapChain(ID3D12Resource* baseResource, const std::wstring name)
            {
                this->AssociateToResource(D3D12Context::g_Device, baseResource, D3D12_RESOURCE_STATE_PRESENT, name);
                
                this->RTV = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12Context::g_Device->CreateRenderTargetView(baseResource, nullptr, this->RTV);
            }

            void D3D12ColorBuffer::GenerateMipMaps(D3D12CommandContext& ctx)
            {
                if (this->NumOfMipMaps == 0)
                {
                    return;
                }
                // @ TODO: Wait till D3D12CommandContext_Compute implemented...
            }

            D3D12_RESOURCE_FLAGS D3D12ColorBuffer::ResourceFlags() const
            {
                // @ Basic flags for resource of ColorBuffer:
                // @	- NONE
                // @	- ALLOW_RENDER_TARGET
                // @	** If this resource hasn't enabled with msaa, then it should allow unordered_access
                return D3D12_RESOURCE_FLAG_NONE | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
                    (this->NumOfFragments == 1) ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : (D3D12_RESOURCE_FLAGS)0;
            }

            void D3D12ColorBuffer::InitializeDerivedViews(ID3D12Device* device, DXGI_FORMAT format, u32 arraySize, u32 numOfMips)
            {
                this->NumOfMipMaps = numOfMips - 1;

                D3D12_RENDER_TARGET_VIEW_DESC       rtvDesc = {};
                D3D12_SHADER_RESOURCE_VIEW_DESC     srvDesc = {};
                D3D12_UNORDERED_ACCESS_VIEW_DESC    uavDesc = {};

                D3D12ColorBuffer::FillDerivedViewDescs_Default(
                    /*ref param*/rtvDesc, /*ref param*/uavDesc, /*ref param*/srvDesc,
                    arraySize, numOfMips, format, this->NumOfFragments > 1
                );

                if (this->SRV.ptr == -1)
                {
                    this->SRV = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    this->RTV = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                }
                D3D12Context::g_Device->CreateShaderResourceView(this->pNativeResource, &srvDesc, this->SRV);
                D3D12Context::g_Device->CreateRenderTargetView(this->pNativeResource, &rtvDesc, this->RTV);

                // If we use MSAA, UAV access to texture will not be allowed.
                if (this->NumOfFragments == 1)
                {
                    for (u32 i = 0; i < numOfMips; ++i)
                    {
                        if (this->UAVs[i].ptr == -1)
                        {
                            this->UAVs[i] = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                        }
                        D3D12Context::g_Device->CreateUnorderedAccessView(this->pNativeResource, nullptr, &uavDesc, this->UAVs[i]);

                        uavDesc.Texture2D.MipSlice++;
                    }
                }
            }

            D3D12DepthBuffer::D3D12DepthBuffer(float clearDepth, u8 clearStencil)
                : ClearDepth(clearDepth)
                , ClearStencil(clearStencil)
            {
                this->DSVs[0].ptr = -1;
                this->DSVs[1].ptr = -1;
                this->DSVs[2].ptr = -1;
                this->DSVs[3].ptr = -1;
                this->SRV_Depth.ptr = -1;
                this->SRV_Stencil.ptr = -1;
            }

            void D3D12DepthBuffer::Initialize(u32 width, u32 height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS vidmem, const std::wstring& name)
            {
                D3D12_RESOURCE_DESC resource = this->ResetAndDescribeTex2D(width, height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
                D3D12_CLEAR_VALUE clearValue = {};
                clearValue.Format = format;
                this->InitializeAsTextureResource(D3D12Context::g_Device, resource, clearValue, vidmem, name);
                this->InitializeDerivedViews(D3D12Context::g_Device, format);
            }

            void D3D12DepthBuffer::Initialize(uint32_t Width, uint32_t Height, uint32_t NumSamples, DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr, const std::wstring& Name)
            {
                D3D12_RESOURCE_DESC resourceDesc = this->ResetAndDescribeTex2D(Width, Height, 1, 1, Format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
                resourceDesc.SampleDesc.Count = NumSamples;
                D3D12_CLEAR_VALUE clearVal = {};
                clearVal.Format = Format;
                this->InitializeAsTextureResource(D3D12Context::g_Device, resourceDesc, clearVal, VidMemPtr, Name);
                this->InitializeDerivedViews(D3D12Context::g_Device, Format);
            }

            void D3D12DepthBuffer::InitializeDerivedViews(ID3D12Device* device, DXGI_FORMAT format)
            {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;

                // ViewDimension depends on whether MSAA is enabled.
                if (this->pNativeResource->GetDesc().SampleDesc.Count == 1)
                {
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    dsvDesc.Texture2D.MipSlice = 0;
                }
                else
                {
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
                }

                // Create DSV for Depth
                dsvDesc.Format = this->ClassifyDSVFormat(format);
                if (this->DSVs[0].ptr == -1)
                {
                    this->DSVs[0] = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                    this->DSVs[1] = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                }
                dsvDesc.Flags = D3D12_DSV_FLAG_NONE;    // indicate resource could be written.
                device->CreateDepthStencilView(this->pNativeResource, &dsvDesc, this->DSVs[0]);
                dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
                device->CreateDepthStencilView(this->pNativeResource, &dsvDesc, this->DSVs[1]);

                // Create DSV for Stencil
                DXGI_FORMAT stencilReadFormat = this->ClassifyStencilFormat(format);
                if (stencilReadFormat != DXGI_FORMAT_UNKNOWN)
                {
                    if (this->DSVs[2].ptr == -1)
                    {
                        this->DSVs[2] = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                        this->DSVs[3] = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                    }
                    dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
                    device->CreateDepthStencilView(this->pNativeResource, &dsvDesc, this->DSVs[2]);
                    dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
                    device->CreateDepthStencilView(this->pNativeResource, &dsvDesc, this->DSVs[3]);
                }
                else
                {
                    /* Stencil is not enabled as format suggests*/
                    this->DSVs[2] = this->DSVs[0];
                    this->DSVs[3] = this->DSVs[1];
                }

                // Create SRVs
                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                if (dsvDesc.ViewDimension == D3D12_DSV_DIMENSION_TEXTURE2D)
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = 1;
                }
                else
                {
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
                }

                // -- Create SRV for Depth
                if (this->SRV_Depth.ptr == -1)
                {
                    this->SRV_Depth = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
                srvDesc.Format = this->ClassifyDepthFormat(format);
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                device->CreateShaderResourceView(this->pNativeResource, &srvDesc, this->SRV_Depth);

                // -- Create SRV for Stencil
                if (stencilReadFormat != DXGI_FORMAT_UNKNOWN)
                {
                    if (this->SRV_Stencil.ptr == -1)
                    {
                        this->SRV_Stencil = DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    }
                    srvDesc.Format = stencilReadFormat;
                    device->CreateShaderResourceView(this->pNativeResource, &srvDesc, this->SRV_Stencil);
                }
            }
        }
	}
}
