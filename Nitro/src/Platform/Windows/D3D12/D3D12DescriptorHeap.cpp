#include "NtPCH.h"
#include "D3D12DescriptorHeap.h"
#include "Nitro/Application.h"
#include "D3D12Context.h"
#include "D3D12CommandContext.h"
#include "D3D12RootSignature.h"
#include "D3D12CommandQueue.h"

namespace Nitro
{
	namespace Graphics
	{ 
		namespace dx
		{
			// @ ------------------------------------------------------------------
			// @						DescriptorHandle
			// @ ------------------------------------------------------------------
			D3D12DescriptorHandle::D3D12DescriptorHandle()
			{
				CpuHandle.ptr = NT_DX_UNKNOWN_ADDRESS;
				GpuHandle.ptr = NT_DX_UNKNOWN_ADDRESS;
			}
			D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu)
				: CpuHandle(hCpu)
			{
				GpuHandle.ptr = NT_DX_UNKNOWN_ADDRESS;
			}
			D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu, D3D12_GPU_DESCRIPTOR_HANDLE hGpu)
				: CpuHandle(hCpu), GpuHandle(hGpu)
			{}
			D3D12DescriptorHandle& D3D12DescriptorHandle::operator+=(u32 offsetInByte)
			{
				if (this->CpuHandle.ptr != NT_DX_UNKNOWN_ADDRESS)
					this->CpuHandle.ptr += offsetInByte;
				if (this->GpuHandle.ptr != NT_DX_UNKNOWN_ADDRESS)
					this->GpuHandle.ptr += offsetInByte;
				return *this;
			}
			D3D12DescriptorHandle D3D12DescriptorHandle::operator+(u32 offsetInByte) const
			{
				D3D12DescriptorHandle res = *this;
				res += offsetInByte;
				return res;
			}

#pragma region DescriptorAllocator_CpuOnly
			// @ ------------------------------------------------------------------
			// @				DescriptorAllocator_CpuViewOnly
			// @ ------------------------------------------------------------------
			DescriptorAllocator_CpuViewOnly	DescriptorAllocator_CpuViewOnly::g_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
			{
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				D3D12_DESCRIPTOR_HEAP_TYPE_DSV
			};

			std::vector<ID3D12DescriptorHeap*>	DescriptorAllocator_CpuViewOnly::s_DescriptorHeapPool;
			std::mutex							DescriptorAllocator_CpuViewOnly::s_AllocationMutex;
			const u32							DescriptorAllocator_CpuViewOnly::s_NumDescriptorPerHeap = BIT(8);

			D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator_CpuViewOnly::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count)
			{
				return DescriptorAllocator_CpuViewOnly::g_DescriptorAllocators[type].Allocate(count);
			}

			ID3D12DescriptorHeap* DescriptorAllocator_CpuViewOnly::RequestNewHeap(descriptor_heap_t type)
			{
				std::lock_guard<std::mutex> internal_lock(s_AllocationMutex);
				descriptor_heap_desc desc;
				desc.NodeMask = 0;
				desc.NumDescriptors = s_NumDescriptorPerHeap;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				desc.Type = type;

				ID3D12DescriptorHeap* heapRes;
				HRESULT hr = D3D12Context::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapRes));
				NT_ASSERT(SUCCEEDED(hr), "Descriptor heap creation failed. Error: {0}", Nitro::Util::integral_to_hexstr(hr));

				s_DescriptorHeapPool.emplace_back(heapRes);

				return heapRes;
			}

			void DescriptorAllocator_CpuViewOnly::DestroyPool()
			{
				for (auto nativeHeap : s_DescriptorHeapPool)
				{
					NT_DX_RELEASE(nativeHeap);
				}
				s_DescriptorHeapPool.clear();
			}

			DescriptorAllocator_CpuViewOnly::DescriptorAllocator_CpuViewOnly(descriptor_heap_t type)
				: Type(type)
				, CurrentHeap(nullptr)
				, RemainingFreeHandles(0)
				, DescriptorSize(0)
			{
				CurrentHandle.ptr = NT_DX_UNKNOWN_ADDRESS;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator_CpuViewOnly::Allocate(u32 count)
			{
				if (CurrentHeap == nullptr || RemainingFreeHandles < count)
				{
					this->CurrentHeap = RequestNewHeap(this->Type);
					this->RemainingFreeHandles = DescriptorAllocator_CpuViewOnly::s_NumDescriptorPerHeap;
					this->CurrentHandle = CurrentHeap->GetCPUDescriptorHandleForHeapStart();
					this->DescriptorSize = Nitro::Graphics::dx::D3D12Context::g_Device->GetDescriptorHandleIncrementSize(this->Type);
				}
				D3D12_CPU_DESCRIPTOR_HANDLE res = this->CurrentHandle;
				this->RemainingFreeHandles -= count;
				this->CurrentHandle.ptr += this->DescriptorSize * count;
				return res;
			}
#pragma endregion

#pragma region DescriptorHeap_Static
			// @ ------------------------------------------------------------------
			// @			DescriptorHeap_Static (Pre-defined static size)
			// @ ------------------------------------------------------------------
			D3D12DescriptorHeap_Static::D3D12DescriptorHeap_Static(descriptor_heap_t type, u32 maxCnt)
				: m_FirstHandle()
				, m_NextFreeHandle()
				, m_Heap(nullptr)
				, m_Type(type)
				, m_DescriptorSize(-1)
				, m_RemainingFrees(0)
				, m_StaticHeapSizeInNum(maxCnt)
			{}

			D3D12DescriptorHeap_Static::~D3D12DescriptorHeap_Static()
			{
				NT_DX_RELEASE(this->m_Heap);
			}

			void D3D12DescriptorHeap_Static::Init(const std::string& name, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				// @ ==== Descriptor Heap Flags
				// @		- The flag D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE can optionally be set on a descriptor heap to indicate it is be bound on a command list for reference by shaders.Descriptor heaps created without this flag allow applications the option to stage descriptors in CPU memory before copying them to a shader visible descriptor heap, as a convenience.But it is also fine for applications to directly create descriptors into shader visible descriptor heaps with no requirement to stage anything on the CPU.
				// @		- This flag only applies to CBV, SRV, UAVand samplers.It does not apply to other descriptor heap types since shaders do not directly reference the other types.
				bool errorConditions = (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && flags != D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
					|| (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV && flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
				NT_ASSERT(!errorConditions, "Flag D3D12_DESCRIPTOR_HEAP_FLAG is invalid with this heap type."
				);
				desc.Flags = flags;
				desc.NodeMask = 0;
				desc.NumDescriptors = m_StaticHeapSizeInNum;
				desc.Type = m_Type;

				HRESULT hr = D3D12Context::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap));
				NT_ASSERT(SUCCEEDED(hr), "Descriptor heap creation failed. Error: {0}", Nitro::Util::integral_to_hexstr(hr));
#ifdef NT_DEBUG
				const std::wstring _name = NT_STDSTR_TO_STDWSTR(name);
				m_Heap->SetName(_name.c_str());
#else
				(void)name;
#endif
				this->m_FirstHandle = D3D12DescriptorHandle(m_Heap->GetCPUDescriptorHandleForHeapStart(), m_Heap->GetGPUDescriptorHandleForHeapStart());
				this->m_DescriptorSize = D3D12Context::g_Device->GetDescriptorHandleIncrementSize(this->m_Type);
			}

			bool D3D12DescriptorHeap_Static::HasAvailableSpace(u32 requestNum)
			{
				return m_RemainingFrees >= requestNum;
			}

			D3D12DescriptorHandle D3D12DescriptorHeap_Static::Allocate(u32 requestNum)
			{
				NT_ASSERT(this->HasAvailableSpace(requestNum), "Not enough space in current static heap.");
				D3D12DescriptorHandle res = m_NextFreeHandle;
				m_NextFreeHandle += requestNum * m_DescriptorSize;
				return res;
			}

			bool D3D12DescriptorHeap_Static::ValidateHandle(const D3D12DescriptorHandle& descriptorHandle) const
			{
				// @ Make sure target handle is within range of this heap
				if (descriptorHandle.GetCpuHandle().ptr < m_FirstHandle.GetCpuHandle().ptr
					|| descriptorHandle.GetCpuHandle().ptr > m_FirstHandle.GetCpuHandle().ptr + m_StaticHeapSizeInNum * m_DescriptorSize)
				{
					return false;
				}
				// @ Make sure GPU handle ptr is synced with CPU handle
				if (descriptorHandle.GetCpuHandle().ptr - m_FirstHandle.GetCpuHandle().ptr
					== descriptorHandle.GetGpuHandle().ptr - m_FirstHandle.GetGpuHandle().ptr)
				{
					return false;
				}

				return true;
			}
#pragma endregion

#pragma region DescriptorHeap_Dynamic
			std::vector<ID3D12DescriptorHeap*>						D3D12DescriptorHeap_Dynamic::sm_DescriptorHeapPools[2];
			std::queue<ID3D12DescriptorHeap*>						D3D12DescriptorHeap_Dynamic::sm_AvailableDescriptorHeaps[2];
			std::queue<std::pair<u64, ID3D12DescriptorHeap*>>		D3D12DescriptorHeap_Dynamic::sm_RetiredDescriptorHeaps[2];
			std::mutex												D3D12DescriptorHeap_Dynamic::sm_Mutex;
			
			void D3D12DescriptorHeap_Dynamic::TableHandleCache::ParseRootSignature(const D3D12RootSignature& rs, DescriptorHeapType heapType)
			{
				// @ In the worst scenario, all root params are table (of size 16-handles large),
				// @ and the total size of cache is 256, we could only have 16 root params.
				NT_ASSERT(rs.GetNumOfParams() <= 16, "In danger of descriptor cache overflow.");

				this->BitMap_StagedRootParams = 0;
				this->BitMap_RootParamsSamplerOrView = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ? rs.GetRootDescriptorTableBitMap_Sampler() : rs.GetRootDescriptorTableBitMap_NonSampler();

				unsigned long bitMap_tableParamsOfHeapType = this->BitMap_RootParamsSamplerOrView;
				unsigned long tableIndex = 0;
				u32 offset = 0;
				while (_BitScanForward(&tableIndex, bitMap_tableParamsOfHeapType))
				{
					bitMap_tableParamsOfHeapType ^= (1 << tableIndex);
					
					u32 tableSize = rs.GetRootDescriptorTableSizeAtIdx(tableIndex);
					NT_ASSERT(tableSize > 0, "table param marked as prepared but of size 0");

					this->TableEntries[tableIndex].BitMap_AssignedHandles = 0;
					this->TableEntries[tableIndex].TableSize = tableSize;
					this->TableEntries[tableIndex].TableStart = Handles + offset;
					offset += tableSize;
				}
				this->NumOfCachedDescriptors = offset;
				NT_ASSERT(this->NumOfCachedDescriptors < TableHandleCache::DefaultNum_Descriptors, "Cache overflow.");
			}

			u32 D3D12DescriptorHeap_Dynamic::TableHandleCache::ComputeStagedSize()
			{
				u32 stagedSpace = 0;
				unsigned long rootIndex;
				unsigned long bitMap_Staged = BitMap_StagedRootParams;
				while (_BitScanForward(&rootIndex, bitMap_Staged))
				{
					bitMap_Staged ^= BIT(rootIndex);
					unsigned long maxSetHandle;
					NT_ASSERT(
						_BitScanReverse(&maxSetHandle, TableEntries[rootIndex].BitMap_AssignedHandles) == TRUE,
						"table entry marked as used but no used handle inside."
					);
					stagedSpace += maxSetHandle + 1;
				}
				return stagedSpace;
			}

			void D3D12DescriptorHeap_Dynamic::TableHandleCache::StageDescriptorHandles(u32 rootIndex, u32 offset, u32 numOfHandles, const DescriptorHandle handles[])
			{
				NT_ASSERT(BIT(rootIndex) & this->BitMap_RootParamsSamplerOrView, "Invalid rootIndex to stage input handles.");
				NT_ASSERT((offset + numOfHandles)<=this->TableEntries[rootIndex].TableSize, "Not enough space in indexed table to store input handles.");
				
				TableEntry& entry = this->TableEntries[rootIndex];
				for (u32 i = 0; i < numOfHandles; ++i)
				{
					DescriptorHandle* copyDst = entry.TableStart + offset + i;
					*copyDst = handles[i];
				}
				entry.BitMap_AssignedHandles |= (BIT(numOfHandles) - 1) << offset;
				this->BitMap_StagedRootParams |= BIT(rootIndex);
			}

			void D3D12DescriptorHeap_Dynamic::TableHandleCache::RefreshStagedBitMap()
			{
				this->BitMap_StagedRootParams = 0;
				unsigned long rootIndex = 0;
				unsigned long tempStagedBitMap = this->BitMap_StagedRootParams;
				while (_BitScanForward(&rootIndex, tempStagedBitMap))
				{
					tempStagedBitMap ^= BIT(rootIndex);
					if (this->TableEntries[rootIndex].BitMap_AssignedHandles != 0)
					{
						this->BitMap_StagedRootParams |= BIT(rootIndex);
					}
				}
			}

			void D3D12DescriptorHeap_Dynamic::TableHandleCache::CopyAndBindStagedTables(D3D12DescriptorHandle dest, ID3D12GraphicsCommandList* cl, DescriptorLoadFunc func_loadDescriptor, DescriptorHeapType heapType, u32 descriptorSize)
			{
#pragma region Preparation of Copy Over
				u32 tableSizes[TableHandleCache::MaxNum_DescriptorTable];
				u32 rootIndicies[TableHandleCache::DefaultNum_Descriptors];
				u32 totalNum_stagedParam = 0;

				unsigned long bitMap_stagedParams = this->BitMap_StagedRootParams;
				unsigned long rootIndex;
				while (_BitScanForward(&rootIndex, bitMap_stagedParams))
				{
					bitMap_stagedParams ^= BIT(rootIndex);
					TableEntry& entry = this->TableEntries[rootIndex];

					unsigned long max_set;
					NT_ASSERT(_BitScanReverse(&max_set, entry.BitMap_AssignedHandles) == TRUE, "No assigned handle in current table");

					tableSizes[totalNum_stagedParam] = max_set + 1;
					++totalNum_stagedParam;
				}
				static const u32 CONST_NumOfHandlesPerCopy = 16;

				DescriptorHandle	startsOfDescriptorRanges_dest[CONST_NumOfHandlesPerCopy];
				u32					sizesOfDescriptorRange_dest[CONST_NumOfHandlesPerCopy];
				u32					currentNumOfDescriptorRange_dest = 0;

				DescriptorHandle	startsOfDescriptorRanges_src[CONST_NumOfHandlesPerCopy];
				u32					sizesOfDescriptorRange_src[CONST_NumOfHandlesPerCopy];
				u32					currentNumOfDescriptorRange_src = 0;
#pragma endregion
				NT_ASSERT(
					totalNum_stagedParam < TableHandleCache::MaxNum_DescriptorTable,
					"Table param num({0}) exceeds limit: {1}",
					totalNum_stagedParam, TableHandleCache::MaxNum_DescriptorTable
				);

#pragma region COPY OVER FOR ALL STAGED PARAMS

				for (u32 i = 0; i < totalNum_stagedParam; ++i)
				{
					u32 rootIndex = rootIndicies[i];

#pragma region GPU COPY OVER FOR CURRENT TABLE
					(cl->*func_loadDescriptor)(rootIndex, dest.GetGpuHandle());
#pragma endregion

#pragma region CPU COPY OVER FOR CURRENT TABLE
					TableEntry& processing_tableEntry = this->TableEntries[rootIndex];
					DescriptorHandle* src_handle = processing_tableEntry.TableStart;
					DescriptorHandle dest_handle = dest.GetCpuHandle();

					unsigned long bitmap_assigned = processing_tableEntry.BitMap_AssignedHandles;
					unsigned long skip_count;
					while (_BitScanForward(&skip_count, bitmap_assigned))
					{
						bitmap_assigned >>= skip_count;
						src_handle += skip_count;

						unsigned long assigned_count;
						_BitScanForward(&assigned_count, ~bitmap_assigned);
						bitmap_assigned >>= assigned_count;

						if (currentNumOfDescriptorRange_src + assigned_count > /*Constant*/CONST_NumOfHandlesPerCopy)
						{
							D3D12Context::g_Device->CopyDescriptors(
								currentNumOfDescriptorRange_dest, startsOfDescriptorRanges_dest, sizesOfDescriptorRange_dest
								, currentNumOfDescriptorRange_src, startsOfDescriptorRanges_src, sizesOfDescriptorRange_src, heapType
							);
							currentNumOfDescriptorRange_src = 0;
							currentNumOfDescriptorRange_dest = 0;
						}
						// @ ***** setup copy over dest *****
						startsOfDescriptorRanges_dest[currentNumOfDescriptorRange_dest] = dest_handle;
						sizesOfDescriptorRange_dest[currentNumOfDescriptorRange_dest] = assigned_count;
						++currentNumOfDescriptorRange_dest;

						// @ ***** setup copy over src *****
						for (u32 j = 0; j < assigned_count; ++j)
						{
							startsOfDescriptorRanges_src[j] = src_handle[j];
							sizesOfDescriptorRange_src[j] = 1;
							++currentNumOfDescriptorRange_src;
						}

						src_handle += assigned_count;
						dest_handle.ptr += assigned_count * descriptorSize;
					}

					dest += tableSizes[i] * descriptorSize;
#pragma endregion
				}

				D3D12Context::g_Device->CopyDescriptors(
					currentNumOfDescriptorRange_dest, startsOfDescriptorRanges_dest, sizesOfDescriptorRange_dest
					, currentNumOfDescriptorRange_src, startsOfDescriptorRanges_src, sizesOfDescriptorRange_src, heapType
				);
#pragma endregion

				this->BitMap_StagedRootParams = 0;
			}

			D3D12DescriptorHeap_Dynamic::D3D12DescriptorHeap_Dynamic(D3D12CommandContext& ctrlContext, D3D12_DESCRIPTOR_HEAP_TYPE heapType)
				: m_CtrlContext(ctrlContext)
				, m_HeapType(heapType)
				, m_DescriptorSize(D3D12Context::g_Device->GetDescriptorHandleIncrementSize(heapType))
				, m_CurrentNativeHeap(nullptr)
				, m_CurrentOffset(0)
			{
			}

			void D3D12DescriptorHeap_Dynamic::RetireCurrentHeap()
			{
				// @ Don't retire if the current heap is never used.
				if (m_CurrentOffset == 0)
				{
					NT_ASSERT(m_CurrentNativeHeap == nullptr, "Current native heap is null");
					return;
				}
				
				NT_ASSERT(m_CurrentNativeHeap != nullptr, "Current native heap is null");
				m_RetiredHeaps.push_back(m_CurrentNativeHeap);
				m_CurrentNativeHeap = nullptr;
				m_CurrentOffset = 0;
			}

			void D3D12DescriptorHeap_Dynamic::RefreshStagedBitMap()
			{
				this->m_ComputeTableHandleCache.RefreshStagedBitMap();
				this->m_GraphicsTableHandleCache.RefreshStagedBitMap();
			}

			ID3D12DescriptorHeap* D3D12DescriptorHeap_Dynamic::UpdateNativeHeap()
			{
				if (m_CurrentNativeHeap == nullptr)
				{
					NT_ASSERT(m_CurrentOffset == 0, "Current heap is null, but heap offset hasn't get set to 0");
					m_CurrentOffset = 0;
					m_CurrentNativeHeap = D3D12DescriptorHeap_Dynamic::RequestDescriptorHeap(m_HeapType);
					m_CurrentFirstHandle = D3D12DescriptorHandle(
						m_CurrentNativeHeap->GetCPUDescriptorHandleForHeapStart(),
						m_CurrentNativeHeap->GetGPUDescriptorHandleForHeapStart()
					);
				}
				return m_CurrentNativeHeap;
			}

			bool D3D12DescriptorHeap_Dynamic::HasSpace(u32 count)
			{
				return (m_CurrentNativeHeap != nullptr) && ((m_CurrentOffset + count) < sm_DefaultHeapSizeInCount);
			}

			void D3D12DescriptorHeap_Dynamic::CleanUpAll(const u64& fenceVal)
			{
				this->RetireCurrentHeap();
				D3D12DescriptorHeap_Dynamic::DiscardDescriptorHeaps(this->m_HeapType, fenceVal, this->m_RetiredHeaps);
				
				this->m_RetiredHeaps.clear();
				this->m_GraphicsTableHandleCache.ClearCache();
				this->m_ComputeTableHandleCache.ClearCache();
			}

			ID3D12DescriptorHeap* D3D12DescriptorHeap_Dynamic::RequestDescriptorHeap(DescriptorHeapType heapType)
			{
				std::lock_guard<std::mutex> internal_lock(sm_Mutex);	
				
				u32 i = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ? 1 : 0;

				std::queue<Pair_FenceValToHeap>& retiredHeaps = sm_RetiredDescriptorHeaps[i];
				std::queue<ID3D12DescriptorHeap*>& availbleHeaps = sm_AvailableDescriptorHeaps[i];
				std::vector<ID3D12DescriptorHeap*>& heapPool = sm_DescriptorHeapPools[i];

				while (!retiredHeaps.empty() && D3D12CommandQueueManager::GetInstance()->IsFenceComplete(retiredHeaps.front().first))
				{
					availbleHeaps.push(retiredHeaps.front().second);
					retiredHeaps.pop();
				}
				
				ID3D12DescriptorHeap* res;
				if (!availbleHeaps.empty())
				{
					res = availbleHeaps.front();
					availbleHeaps.pop();
				}
				else
				{
					D3D12_DESCRIPTOR_HEAP_DESC desc = {};
					desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
					desc.NodeMask = 0;
					desc.NumDescriptors = D3D12DescriptorHeap_Dynamic::sm_DefaultHeapSizeInCount;
					desc.Type = heapType;
					HRESULT hr = D3D12Context::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&res));
					NT_ASSERT(SUCCEEDED(hr), "Descriptor heap creation failed.");
					heapPool.push_back(res);
				}

				return res;
			}

			void D3D12DescriptorHeap_Dynamic::DiscardDescriptorHeaps(DescriptorHeapType heapType, const u64 fenceVal, const std::vector<ID3D12DescriptorHeap*>& heaps)
			{
				std::lock_guard<std::mutex> lock_guard(D3D12DescriptorHeap_Dynamic::sm_Mutex);

				u32 i = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ? 1 : 0;
				std::queue<Pair_FenceValToHeap>& retiredHeaps = D3D12DescriptorHeap_Dynamic::sm_RetiredDescriptorHeaps[i];
				for (ID3D12DescriptorHeap* toBeDiscard : heaps)
				{
					retiredHeaps.push(std::make_pair(fenceVal, toBeDiscard));
				}
			}

			void D3D12DescriptorHeap_Dynamic::DestroyAll()
			{
				for (u32 i = 0; i < 2; ++i)
				{
					std::vector<ID3D12DescriptorHeap*>& currentPool = sm_DescriptorHeapPools[i];
					for (ID3D12DescriptorHeap* heap : currentPool)
					{
						NT_DX_RELEASE(heap);
					}
					currentPool.clear();
				}
			}

			D3D12DescriptorHandle D3D12DescriptorHeap_Dynamic::Allocate(u32 count)
			{
				D3D12DescriptorHandle res = m_CurrentFirstHandle + m_CurrentOffset * m_DescriptorSize;
				m_CurrentOffset += count;
				return res;
			}

			void D3D12DescriptorHeap_Dynamic::CopyAndBindStagedTables(TableHandleCache& handleCache, ID3D12GraphicsCommandList* cl, DescriptorLoadFunc descriptorLoadFunc)
			{
				u32 neededSpace = handleCache.ComputeStagedSize();
				if (!this->HasSpace(neededSpace))
				{
					this->RetireCurrentHeap();
					// this->RefreshStagedBitMap();
					// neededSpace = handleCache.ComputeStagedSize();
				}

				m_CtrlContext.SetDescriptorHeap(m_HeapType, this->UpdateNativeHeap());
				handleCache.CopyAndBindStagedTables(this->Allocate(neededSpace), cl, descriptorLoadFunc, m_HeapType, m_DescriptorSize);
			}
#pragma endregion

		}
	}
}