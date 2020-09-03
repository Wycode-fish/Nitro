#pragma once

#include "D3D12GpuResource.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{

#define GPU_MEMBLK_DEFAULT_ALIGNMENT 256

			// @ ---------------------------------------------------------------------
			// @							GpuMemoryBlock
			// @ Intro:
			// @ *	A mem block size could be > or < than a page, it's dynamically set
			// @ ---------------------------------------------------------------------
			struct GpuMemoryBlock
			{
#pragma region Variables
				D3D12GpuResource& ContextBuffer;
				size_t Offset;
				size_t Size;
				void* CpuAddress;
				D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
#pragma endregion
				GpuMemoryBlock(D3D12GpuResource& baseResource, const size_t& offset, const size_t& blkSize)
					: ContextBuffer(baseResource), Offset(offset), Size(blkSize)
				{}
			};

			// @ ---------------------------------------------------------------------
			// @							GpuMemoryPage
			// @ Intro:
			// @ * it's at essense a D3D12GpuResource
			// @ ---------------------------------------------------------------------
			enum PageType
			{
				Invalid = -1,
				GpuExclusive,
				CpuWritable,
				NumOfTypes
			};
			enum class DefaultPageSize
			{
				GpuExclusive = 0x10000,	// 64KB, 2^16
				CpuWritable = 0x200000, // 2MB, 2^21
			};
			struct GpuMemoryPage : public D3D12GpuResource
			{
#pragma region Variables
				// @ THIS IS NOT pNativeResource, BUT THE VIRTUAL CPU MAPPED FROM THE pNativeResource.
				void* CpuAddress;
				// @ GPU address is already stored in D3D12GpuResource
#pragma endregion
				GpuMemoryPage(ID3D12Resource* nativeResource, D3D12_RESOURCE_STATES usage)
					: D3D12GpuResource(nativeResource, usage)
					, CpuAddress(nullptr)
				{
					this->Map();
				}
				~GpuMemoryPage() 
				{ 
					this->Unmap(); 
				}
				void Map()
				{
					if (this->CpuAddress == nullptr)
					{
						pNativeResource->Map(0, nullptr, &this->CpuAddress);
					}
				}
				void Unmap()
				{
					if (this->CpuAddress != nullptr)
					{
						pNativeResource->Unmap(0, nullptr);
						this->CpuAddress = nullptr;
					}
				}
			};

			// @ ---------------------------------------------------------------------
			// @						GpuMemoryPageManager
			// @ ---------------------------------------------------------------------
			class GpuMemoryPageManager
			{
				typedef std::pair<u64, GpuMemoryPage*> Pair_FenceVal2Page;
			private:
				static PageType sm_NextPageTypeToInit;
				static void DefaultInit_HeapProps(D3D12_HEAP_PROPERTIES& heapProps);
				static void DefaultInit_ResourceDesc(D3D12_RESOURCE_DESC& resourceDesc);

			public:
				GpuMemoryPageManager();
				void Destroy();
				
				// @ Request:	Returns fixed-size memory page.
				// @ Create:	Could return random-size memory page.	
				GpuMemoryPage* RequestPage();
				GpuMemoryPage* CreateNewPage(const size_t& pageSz = 0);

				// @ Memory page recycle is different for page of large size than which of default size:
				// @	- Default size page ==> Queue_RetiredPages, 
				// @			* which store pages without detroying them, wait for reuse.
				// @	- Large size page	==> Queue_RetiredLargePages, 
				// @			* which delete memory pages when fence value reached.
				void DiscardPages(const u64& fenceVal, const std::vector<GpuMemoryPage*>& toBeDiscard);
				void DestroyLargePages(const u64& fenceVal, const std::vector<GpuMemoryPage*>& toBeDestroy);
			
			private:
				std::vector<std::unique_ptr<GpuMemoryPage> >	m_PagePool;
				std::queue<Pair_FenceVal2Page>					m_RetiredPages;
				std::queue<Pair_FenceVal2Page>					m_RetiredLargePages;
				std::queue<GpuMemoryPage*>						m_AvailablePages;
				std::mutex										m_Mutex;
				PageType										m_PageType;
			};
			
			// @ -------------------------------------------------------------------------
			// @						GpuMemoryAllocator
			// @ Intro:
			// @	* Information struct for page manager
			// @	* 1 page manager  ==> N allocator(s)
			// @	* All fixed-size pages actually live in page manager
			// @	* All large pages actually live in memory allocator, until
			// @		Cleanup stage they got recycled into page manager.
			// @ 	* Constant blocks must be multiples of 16 constants @ 16 bytes each, 
			// @		which is 256
			// @ -------------------------------------------------------------------------

			class GpuMemoryAllocator_Linear
			{
			public:
				static GpuMemoryPageManager g_GpuMemoryPageManager[2];
				static void DetroyAll();
			public:
				GpuMemoryAllocator_Linear(PageType pageType);
				GpuMemoryBlock Allocate(const size_t& sizeInBytes, const size_t& alignment = GPU_MEMBLK_DEFAULT_ALIGNMENT);
				void CleanupUsedPages(const u64& fenceVal);
			private:
				GpuMemoryBlock AllocateLargePage(const size_t& sizeInBytes);
			private:
				std::vector<GpuMemoryPage*> m_ExistedLargePages; // using vector instead of queue here to work with page manager
				std::vector<GpuMemoryPage*> m_RetiredPages;
				size_t						m_PageSize;
				size_t						m_CurrentOffset;
				GpuMemoryPage*				m_CurrentPage;
				PageType					m_PageType;
			};
		}
	}
}