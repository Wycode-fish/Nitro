#include "NtPCH.h"

namespace Nitro
{
	namespace Util
	{
		namespace DX
		{
			u64 GetRequiredIntermediateSize(
				_In_												ID3D12Resource* pDestRes,
				_In_range_(0, D3D12_REQ_SUBRESOURCES)				u32				firstSubres,
				_In_range_(0, D3D12_REQ_SUBRESOURCES - firstSubres) u32				numOfSubres)
			{
				D3D12_RESOURCE_DESC destDesc = pDestRes->GetDesc();

				ID3D12Device* device;
				pDestRes->GetDevice(IID_PPV_ARGS(&device));

				u64 requiredSize;
				device->GetCopyableFootprints(&destDesc, firstSubres, numOfSubres, 0, nullptr, nullptr, nullptr, &requiredSize);

				device->Release();

				return requiredSize;
			}

			void MemcpySubresource(
				_In_	const D3D12_MEMCPY_DEST*		pDest,
				_In_	const D3D12_SUBRESOURCE_DATA*	pSrc,
						u64								RowSizeInBytes,
						u32								NumRows,
						u32								NumSlices)
			{
				for (u32 sliceIndex = 0; sliceIndex < NumSlices; ++sliceIndex)
				{
					unsigned char* sliceDestAddr = (unsigned char*)pDest->pData + sliceIndex * pDest->SlicePitch;
					const unsigned char* sliceSrcAddr = (unsigned char*)pSrc->pData + sliceIndex * pSrc->SlicePitch;
					for (u32 rowIndex = 0; rowIndex < NumRows; ++rowIndex)
					{
						unsigned char* rowDestAddr = sliceDestAddr + rowIndex * pDest->RowPitch;
						const unsigned char* rowSrcAddr = sliceSrcAddr + rowIndex * pSrc->RowPitch;
						memcpy(rowDestAddr, rowSrcAddr, RowSizeInBytes);
					}
				}
			}

			u64 UpdateSubresources(
				_In_												ID3D12GraphicsCommandList*					cl,
				_In_												ID3D12Resource*								pDestRes,
				_In_												ID3D12Resource*								pIntermediateRes,
				_In_range_(0, D3D12_REQ_SUBRESOURCES)				u32											firstSubres,
				_In_range_(0, D3D12_REQ_SUBRESOURCES - firstSubres) u32											numOfSubres,
																	u64											requiredSize,
				_In_reads_(numOfSubres)								const D3D12_PLACED_SUBRESOURCE_FOOTPRINT*	pFootPrints,
				_In_reads_(numOfSubres)								const u32*									pNumRows,
				_In_reads_(numOfSubres)								const u64*									pRowSizesInBytes,
				_In_reads_(numOfSubres)								const D3D12_SUBRESOURCE_DATA*				pData)
			{
#pragma region Resource Validation
				D3D12_RESOURCE_DESC intermediateDesc = pIntermediateRes->GetDesc();
				D3D12_RESOURCE_DESC destDesc = pDestRes->GetDesc();

				bool failureCondForIntermediate =
					intermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
					intermediateDesc.Width < requiredSize + pFootPrints[0].Offset ||
					requiredSize >(u64)(-1);

				bool failureCondForDest = destDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER && (numOfSubres != 1 || firstSubres != 0);

				if (failureCondForDest || failureCondForIntermediate)
				{
					return 0;
				}
#pragma endregion

#pragma region Memcpy Subresources Data To Upload Heap
				unsigned char* cpuAddr;
				HRESULT hr = pIntermediateRes->Map(0, nullptr, (void**)&cpuAddr);
				if (FAILED(hr)) return 0;

				for (u32 subResIndex = 0; subResIndex < numOfSubres; ++subResIndex)
				{
					u64 rowSizeForCurrentSubres = pRowSizesInBytes[subResIndex];
					if (rowSizeForCurrentSubres > (u64)-1) return 0;

					// @ Diff between FootPrint.RowPitch and RowSizeInBytes : 
					// @ https://www.gamedev.net/forums/topic/677932-getcopyablefootprints-question/
					D3D12_MEMCPY_DEST memcpyDest = {};
					memcpyDest.pData = cpuAddr + pFootPrints[subResIndex].Offset;
					memcpyDest.RowPitch = pFootPrints[subResIndex].Footprint.RowPitch;
					memcpyDest.SlicePitch = (u64)pFootPrints[subResIndex].Footprint.RowPitch * (u64)pNumRows[subResIndex];

					Util::DX::MemcpySubresource(
						&memcpyDest,
						pData + subResIndex,
						pRowSizesInBytes[subResIndex],
						pNumRows[subResIndex], pFootPrints[subResIndex].Footprint.Depth);
				}
				pIntermediateRes->Unmap(0, nullptr);
#pragma endregion

#pragma region Copy Data To Default Heap
				if (destDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
				{
					cl->CopyBufferRegion(pDestRes, 0, pIntermediateRes, pFootPrints[0].Offset, pFootPrints[0].Footprint.Width);
				}
				else
				{
					for (u32 i = 0; i < numOfSubres; ++i)
					{
						D3D12_TEXTURE_COPY_LOCATION destLocation = {};
						destLocation.pResource = pDestRes;
						destLocation.SubresourceIndex = firstSubres + i;
						D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
						srcLocation.pResource = pIntermediateRes;
						srcLocation.PlacedFootprint = pFootPrints[i];

						cl->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
					}
				}
#pragma endregion

				return requiredSize;
			}

			u64 UpdateSubresources(
				_In_												ID3D12GraphicsCommandList*		cl,
				_In_												ID3D12Resource*					pDestRes,
				_In_												ID3D12Resource*					pIntermediateRes,
				_In_range_(0, D3D12_REQ_SUBRESOURCES)				u32								firstSubres,
				_In_range_(0, D3D12_REQ_SUBRESOURCES - firstSubres) u32								numOfSubres,
																	u64								intermediateOffset,
				_In_reads_(numOfSubres)								const D3D12_SUBRESOURCE_DATA*	pData)
			{
				u64 requiredSize = 0;
				u64 footprintMemorySize = static_cast<u64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(u64) + sizeof(u32)) * numOfSubres;
				if (footprintMemorySize > SIZE_MAX) return 0;

				void* footprintMemory = HeapAlloc(GetProcessHeap(), 0, footprintMemorySize);
				if (footprintMemory == nullptr) return 0;

				D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pFootprints		= reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(footprintMemory);
				u64*								pRowSizesInBytes = reinterpret_cast<u64*>(pFootprints + numOfSubres);
				u32*								pNumOfRows		= reinterpret_cast<u32*>(pRowSizesInBytes + numOfSubres);

				D3D12_RESOURCE_DESC destDesc = pDestRes->GetDesc();
				ID3D12Device* device;
				pDestRes->GetDevice(IID_PPV_ARGS(&device));
				device->GetCopyableFootprints(&destDesc, firstSubres, numOfSubres, intermediateOffset, pFootprints, pNumOfRows, pRowSizesInBytes, &requiredSize);
				device->Release();

				u64 res = Util::DX::UpdateSubresources(cl, pDestRes, pIntermediateRes, firstSubres, numOfSubres, requiredSize, pFootprints, pNumOfRows, pRowSizesInBytes, pData);
				HeapFree(GetProcessHeap(), 0, footprintMemory);

				return res;
			}
		}
	}
}