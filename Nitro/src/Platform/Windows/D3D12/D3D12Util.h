#pragma once

// @ -------------------------------------------------------------------------------
// @	Util functions provided in d3dx12.h
// @	* Learning purpose
// @ -------------------------------------------------------------------------------

#include <d3d12.h>

typedef unsigned long long u64;
typedef unsigned int u32;

namespace Nitro
{
	namespace Util
	{
		namespace DX
		{
			// @ -----------------------------------------------------------------------------
			// @							TEXTURE_COPY_LOCATION
			// @ -----------------------------------------------------------------------------
			struct TEXTURE_COPY_LOCATION : public D3D12_TEXTURE_COPY_LOCATION
			{
				// @ 2 Types of texture copy:
				// @	- Copy by subresource index
				// @	- Copy by subresource footprint
				TEXTURE_COPY_LOCATION() = default;

				explicit TEXTURE_COPY_LOCATION(const D3D12_TEXTURE_COPY_LOCATION& native)
					: D3D12_TEXTURE_COPY_LOCATION(native)
				{
				}
				TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* nativeRes)
				{
					this->pResource = nativeRes;
					this->PlacedFootprint = {};
					this->Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				}
				TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* nativeRes, u32 subresIndex)
				{
					this->pResource = nativeRes;
					this->SubresourceIndex = subresIndex;
					this->Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				}
				TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* nativeRes, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint)
				{
					this->pResource = nativeRes;
					this->Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
					this->PlacedFootprint = footprint;
				}
			};
			// @ -----------------------------------------------------------------------------
			// @								HEAP_PROPS
			// @ -----------------------------------------------------------------------------
			struct HEAP_PROPS : public D3D12_HEAP_PROPERTIES
			{
				HEAP_PROPS() = default;

				explicit HEAP_PROPS(const D3D12_HEAP_PROPERTIES& props)
					: D3D12_HEAP_PROPERTIES(props)
				{}
				explicit HEAP_PROPS(D3D12_HEAP_TYPE type, u32 creationNodeMask = 0, u32 nodeMask = 0)
				{
					this->Type = type;
					this->CreationNodeMask = creationNodeMask;
					this->VisibleNodeMask = nodeMask;
					this->CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					this->MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				}
				// @ defualt type CUSTOM
				HEAP_PROPS(D3D12_CPU_PAGE_PROPERTY cpuPageProp, D3D12_MEMORY_POOL memoryPoolPreference, u32 creationNodeMask = 0, u32 nodeMask = 0)
				{
					this->CPUPageProperty = cpuPageProp;
					this->CreationNodeMask = creationNodeMask;
					this->MemoryPoolPreference = memoryPoolPreference;
					this->VisibleNodeMask = nodeMask;
					this->Type = D3D12_HEAP_TYPE_CUSTOM;
				}
				bool IsCpuAccessible() const
				{
					// CpuPageProperty matters: Writeback or WriteCombine
					return this->Type == D3D12_HEAP_TYPE_UPLOAD 
						|| this->Type == D3D12_HEAP_TYPE_READBACK
						|| ((this->Type == D3D12_HEAP_TYPE_CUSTOM) && (this->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK || this->CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE));
				}
			};
			inline bool operator==(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
			{
				return l.Type == r.Type &&
					l.CPUPageProperty == r.CPUPageProperty &&
					l.CreationNodeMask == r.CreationNodeMask &&
					l.MemoryPoolPreference == r.MemoryPoolPreference &&
					l.VisibleNodeMask == r.VisibleNodeMask;
			}
			inline bool operator!=(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
			{
				return !(l == r);
			}
			// @ -----------------------------------------------------------------------------
			// @								Helper Funcs
			// @ -----------------------------------------------------------------------------
			extern u64 GetRequiredIntermediateSize(
				_In_												ID3D12Resource* pDestRes,
				_In_range_(0, D3D12_REQ_SUBRESOURCES)				u32				firstSubres,
				_In_range_(0, D3D12_REQ_SUBRESOURCES - firstSubres) u32				numOfSubres);

			extern void MemcpySubresource(
				_In_	const D3D12_MEMCPY_DEST*		pDest,
				_In_	const D3D12_SUBRESOURCE_DATA*	pSrc,
						u64								RowSizeInBytes,
						u32								NumRows,
						u32								NumSlices);

			extern u64 UpdateSubresources(
				_In_												ID3D12GraphicsCommandList*					cl,
				_In_												ID3D12Resource*								pDestRes,
				_In_												ID3D12Resource*								pIntermediateRes,
				_In_range_(0, D3D12_REQ_SUBRESOURCES)				u32											firstSubres,
				_In_range_(0, D3D12_REQ_SUBRESOURCES - firstSubres) u32											numOfSubres,
																	u64											intermediateOffset,
				_In_reads_(numOfSubres)								const D3D12_SUBRESOURCE_DATA*				pData);
		}
	}
}