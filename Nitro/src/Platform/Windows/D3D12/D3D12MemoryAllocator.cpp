#include "NtPCH.h"
#include "D3D12MemoryAllocator.h"
#include "D3D12CommandQueue.h"
#include "D3D12Context.h"

#include "Nitro/Util/AlignOps.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			// @ ---------------------------------------------------------------------
			// @						GpuMemoryPageManager
			// @ ---------------------------------------------------------------------
#pragma region Static Variables / Methods
			PageType GpuMemoryPageManager::sm_NextPageTypeToInit = PageType::GpuExclusive;

			void GpuMemoryPageManager::DefaultInit_HeapProps(D3D12_HEAP_PROPERTIES& heapProps)
			{
				heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				heapProps.CreationNodeMask = 0;
				heapProps.VisibleNodeMask = 0;
			}

			void GpuMemoryPageManager::DefaultInit_ResourceDesc(D3D12_RESOURCE_DESC& resourceDesc)
			{
				resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
				resourceDesc.Alignment = 0;
				resourceDesc.DepthOrArraySize = 1;
				resourceDesc.Height = 1;
				resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				resourceDesc.MipLevels = 1;
				resourceDesc.SampleDesc.Count = 1;
				resourceDesc.SampleDesc.Quality = 0;
				resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			}
#pragma endregion


			GpuMemoryPageManager::GpuMemoryPageManager()
			{
				m_PageType = sm_NextPageTypeToInit;
				sm_NextPageTypeToInit = (PageType)(m_PageType + 1);
				NT_ASSERT(sm_NextPageTypeToInit <= PageType::NumOfTypes, "Current page type is invalid to initialize.");
			}

			GpuMemoryPage* GpuMemoryPageManager::RequestPage()
			{
				std::lock_guard<std::mutex> internal_lock(m_Mutex);

				while (!m_RetiredPages.empty() && D3D12CommandQueueManager::GetInstance()->IsFenceComplete(m_RetiredPages.front().first))
				{
					m_AvailablePages.push(m_RetiredPages.front().second);
					m_RetiredPages.pop();
				}

				GpuMemoryPage* res = nullptr;
				if (!m_AvailablePages.empty())
				{
					res = m_AvailablePages.front();
					m_AvailablePages.pop();
				}
				else
				{
					res = this->CreateNewPage();
					m_PagePool.emplace_back(res);
				}
				return res;
			}

			GpuMemoryPage* GpuMemoryPageManager::CreateNewPage(const size_t& pageSz)
			{
				// @ Pre-assign every field of resource description except:
				// @	- Type (Depends on usage of heap)
				D3D12_HEAP_PROPERTIES heapProps;
				GpuMemoryPageManager::DefaultInit_HeapProps(heapProps);

				// @ Pre-assign every field of resource description except:
				// @	- Flags (Depends on resource usage)
				// @	- Width (Depends on input param)
				D3D12_RESOURCE_DESC resourceDesc;
				GpuMemoryPageManager::DefaultInit_ResourceDesc(resourceDesc);

				D3D12_RESOURCE_STATES initUsage;
				if (m_PageType == PageType::GpuExclusive)
				{
					heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
					resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
					resourceDesc.Width = (pageSz == 0) ? (size_t)DefaultPageSize::GpuExclusive : pageSz;
					initUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
				else // Cpu-Writable resource
				{
					heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
					resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
					resourceDesc.Width = (pageSz == 0) ? (size_t)DefaultPageSize::CpuWritable : pageSz;
					initUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
				}

				ID3D12Resource* memoryPage;
				HRESULT hr = D3D12Context::g_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initUsage, nullptr, IID_PPV_ARGS(&memoryPage));
				NT_ASSERT(SUCCEEDED(hr), "Commited resource creation failed.");

				return new GpuMemoryPage(memoryPage, initUsage);
			}

			void GpuMemoryPageManager::DiscardPages(const u64& fenceVal, const std::vector<GpuMemoryPage*>& toBeDiscard)
			{
				std::lock_guard<std::mutex> internal_lock(m_Mutex);
				for (GpuMemoryPage* page : toBeDiscard)
				{
					m_RetiredPages.push(std::make_pair(fenceVal, page));
				}
			}

			void GpuMemoryPageManager::DestroyLargePages(const u64& fenceVal, const std::vector<GpuMemoryPage*>& toBeDestroy)
			{
				std::lock_guard<std::mutex> internal_lock(m_Mutex);
				// @ Actual deletion of previous large pages
				while (!m_RetiredLargePages.empty() && D3D12CommandQueueManager::GetInstance()->IsFenceComplete(m_RetiredLargePages.front().first))
				{
					delete m_RetiredLargePages.front().second;
					m_RetiredLargePages.pop();
				}

				for (GpuMemoryPage* page : toBeDestroy)
				{
					page->Unmap(); // @ Could be removed, as unmap will get called in dtor
					m_RetiredLargePages.push(std::make_pair(fenceVal, page));
				}
			}

			void GpuMemoryPageManager::Destroy()
			{
				m_PagePool.clear();
			}

			// @ ---------------------------------------------------------------------
			// @						GpuMemoryPageManager
			// @ ---------------------------------------------------------------------
#pragma region Static Variables / Methods
			GpuMemoryPageManager GpuMemoryAllocator_Linear::g_GpuMemoryPageManager[2];
			void GpuMemoryAllocator_Linear::DetroyAll()
			{
				g_GpuMemoryPageManager[0].Destroy();
				g_GpuMemoryPageManager[1].Destroy();
			}
#pragma endregion
			GpuMemoryAllocator_Linear::GpuMemoryAllocator_Linear(PageType pageType)
				: m_PageSize(0), 
				m_CurrentOffset(0), 
				m_CurrentPage(nullptr), 
				m_PageType(pageType)
			{
				m_PageSize = (pageType == PageType::GpuExclusive) ? (size_t)DefaultPageSize::GpuExclusive : (size_t)DefaultPageSize::CpuWritable;
			}

			GpuMemoryBlock GpuMemoryAllocator_Linear::Allocate(const size_t& sizeInBytes, const size_t& alignment)
			{
				const size_t alignmentMask = alignment - 1;
				NT_ASSERT((alignment & alignmentMask) == 0, "allocate alignment is not power of 2");

				// @ Request size needs to be adjusted to power of 2.
				const size_t alignedSize = Util::alignUp(sizeInBytes, alignmentMask);
				if (alignedSize > m_PageSize)
				{
					return this->AllocateLargePage(alignedSize);
				}

				m_CurrentOffset = Util::alignUp(m_CurrentOffset, alignment);
				if (m_CurrentOffset + alignedSize > m_PageSize)
				{
					NT_ASSERT(m_CurrentPage != nullptr, "Current page hasn't been initialized yet.");
					m_RetiredPages.push_back(m_CurrentPage);
					m_CurrentPage = nullptr;
				}
				if (m_CurrentPage == nullptr)
				{
					m_CurrentPage = GpuMemoryAllocator_Linear::g_GpuMemoryPageManager[(u32)m_PageType].RequestPage();
					m_CurrentOffset = 0;
				}
				GpuMemoryBlock memblk(*m_CurrentPage, m_CurrentOffset, alignedSize);
				memblk.CpuAddress = (unsigned char*)m_CurrentPage->CpuAddress + m_CurrentOffset;
				memblk.GpuAddress = m_CurrentPage->GpuAddress + m_CurrentOffset;
				m_CurrentOffset += alignedSize;
				return memblk;
			}

			void GpuMemoryAllocator_Linear::CleanupUsedPages(const u64& fenceVal)
			{
				if (m_CurrentPage == nullptr)
				{
					return;
				}
				m_RetiredPages.emplace_back(m_CurrentPage);
				m_CurrentPage = nullptr;
				m_CurrentOffset = 0;

				GpuMemoryAllocator_Linear::g_GpuMemoryPageManager[m_PageType].DiscardPages(fenceVal, m_RetiredPages);
				m_RetiredPages.clear();
				GpuMemoryAllocator_Linear::g_GpuMemoryPageManager[m_PageType].DestroyLargePages(fenceVal, m_ExistedLargePages);
				m_ExistedLargePages.clear();
			}

			GpuMemoryBlock GpuMemoryAllocator_Linear::AllocateLargePage(const size_t& sizeInBytes)
			{
				GpuMemoryPage* dynamicPage = GpuMemoryAllocator_Linear::g_GpuMemoryPageManager[this->m_PageType].CreateNewPage(sizeInBytes);
				this->m_ExistedLargePages.push_back(dynamicPage);

				GpuMemoryBlock memblk(*dynamicPage, 0, sizeInBytes);
				memblk.CpuAddress = dynamicPage->CpuAddress;
				memblk.GpuAddress = dynamicPage->GpuAddress;
				return memblk;
			}

		}
	}
}