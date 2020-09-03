#pragma once

#include <d3d12.h>
#include "D3D12RootSignature.h"

#define NT_DX_UNKNOWN_ADDRESS -1

namespace Nitro
{ 
	namespace Graphics
	{
		namespace dx
		{
			// @ ------------------------------------------------------------------
			// @						DescriptorHandle
			// @ ------------------------------------------------------------------
			struct D3D12DescriptorHandle
			{
#pragma region Variables
				D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
				D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
#pragma endregion

#pragma region Constructor
				D3D12DescriptorHandle();
				D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu);
				D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu, D3D12_GPU_DESCRIPTOR_HANDLE hGpu);
#pragma endregion

#pragma region Control Methods
				D3D12DescriptorHandle& operator+=(u32 offsetInByte);
				D3D12DescriptorHandle operator+(u32 offsetInByte) const;

				inline D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return CpuHandle; }
				inline D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return GpuHandle; }

				inline bool IsNull() const { return CpuHandle.ptr == NT_DX_UNKNOWN_ADDRESS; }
				inline bool IsShaderVisible() const { return GpuHandle.ptr != NT_DX_UNKNOWN_ADDRESS; }
#pragma endregion
			};

			// @ ------------------------------------------------------------------
			// @				DescriptorAllocator_CpuViewOnly
			// @ ------------------------------------------------------------------
			struct DescriptorAllocator_CpuViewOnly
			{
				typedef D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
				typedef D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_t;

#pragma region Member Variables
				static DescriptorAllocator_CpuViewOnly	g_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

				ID3D12DescriptorHeap*					CurrentHeap;
				D3D12_CPU_DESCRIPTOR_HANDLE				CurrentHandle;
				D3D12_DESCRIPTOR_HEAP_TYPE				Type;
				u32										DescriptorSize;
				u32										RemainingFreeHandles;
#pragma endregion

#pragma region Static Variables
			private:
				static std::vector<ID3D12DescriptorHeap*>	s_DescriptorHeapPool;
				static std::mutex							s_AllocationMutex;
				static const u32							s_NumDescriptorPerHeap;
#pragma endregion

#pragma region Static Methods
			public:
				static D3D12_CPU_DESCRIPTOR_HANDLE	AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count = 1);
				static ID3D12DescriptorHeap*		RequestNewHeap(descriptor_heap_t type);
				static void							DestroyPool();
#pragma endregion

#pragma region Control Methods
				DescriptorAllocator_CpuViewOnly(descriptor_heap_t type);
				D3D12_CPU_DESCRIPTOR_HANDLE		Allocate(u32 count);
#pragma endregion
			};

			// @ ------------------------------------------------------------------
			// @			DescriptorHeap_Static (user-defined static size)
			// @ ------------------------------------------------------------------
			class D3D12DescriptorHeap_Static
			{
			public:
				typedef D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_t;
				D3D12DescriptorHeap_Static(descriptor_heap_t type, u32 maxCnt);
				~D3D12DescriptorHeap_Static();
			public:
				void					Init(const std::string& name, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
				bool					HasAvailableSpace(u32 requestNum);
				D3D12DescriptorHandle	Allocate(u32 requestNum = 1);
				bool					ValidateHandle(const D3D12DescriptorHandle& descriptorHandle) const;
			public:
				inline D3D12DescriptorHandle GetHandleAtOffset(u32 offsetInNum) const
				{
					return m_FirstHandle + offsetInNum * m_DescriptorSize;
				}
				inline ID3D12DescriptorHeap* GetNativeHeap() const
				{
					return m_Heap;
				}
				inline u32 GetDescriptorSize()
				{
					return m_DescriptorSize;
				}
			private:
				D3D12DescriptorHandle	m_FirstHandle;
				D3D12DescriptorHandle	m_NextFreeHandle;
				ID3D12DescriptorHeap*	m_Heap;
				descriptor_heap_t		m_Type;
				u32						m_DescriptorSize;
				u32						m_RemainingFrees;
				u32						m_StaticHeapSizeInNum;
			};


			// @ --------------------------------------------------------------------------------------
			// @			DescriptorHeap_Dynamic (automatically request new heap)
			// @ Intro:
			// @	* Each dynamic heap specifically works with either Samplers OR Views
			// @		e.g. say if a DynamicHeap initialized with HEAP_TYPE = SAMPLERS, then:
			// @				- It can only used to Store/Stage/Sumit handles of Samplers.
			// @				- It can parse and work *simultaneously* with 1 GraphicsRS and 1 ComputeRS.
			// @ --------------------------------------------------------------------------------------
			class D3D12DescriptorHeap_Dynamic
			{
				friend class D3D12CommandContext;

				typedef void(__stdcall ID3D12GraphicsCommandList::* DescriptorLoadFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE);
				typedef std::pair<u64, ID3D12DescriptorHeap*>	Pair_FenceValToHeap;
				typedef D3D12_DESCRIPTOR_HEAP_TYPE				DescriptorHeapType;
				typedef D3D12_CPU_DESCRIPTOR_HANDLE				DescriptorHandle;

#pragma region Inner Class
				/// <summary>
				/// Info struct for each Descriptor Table
				/// </summary>
				struct TableEntry
				{
					u32 BitMap_AssignedHandles = 0;	// hence, max num of handles a descriptor table can have is 32.
					u32 TableSize = 0;
					D3D12_CPU_DESCRIPTOR_HANDLE* TableStart = nullptr;
				};

				/// <summary>
				/// Memory chunk for descriptor handles
				/// </summary>
				struct TableHandleCache
				{
					static const u32 DefaultNum_Descriptors = 256;
					static const u32 MaxNum_DescriptorTable = 16;

					DescriptorHandle	Handles[DefaultNum_Descriptors];
					TableEntry			TableEntries[MaxNum_DescriptorTable];

					u32					BitMap_RootParamsSamplerOrView;
					u32					BitMap_StagedRootParams;
					u32					NumOfCachedDescriptors;

					inline TableHandleCache() { this->ClearCache(); }
					inline void	ClearCache()
					{
						this->BitMap_RootParamsSamplerOrView = 0;
						this->BitMap_StagedRootParams = 0;
						this->NumOfCachedDescriptors = 0;
					}
					
					void	ParseRootSignature(const D3D12RootSignature& rs, DescriptorHeapType heapType);
					u32		ComputeStagedSize();
					void	StageDescriptorHandles(u32 rootIndex, u32 offset, u32 numOfHandles, const DescriptorHandle handles[]);
					void	RefreshStagedBitMap();
					void	CopyAndBindStagedTables(D3D12DescriptorHandle dest, ID3D12GraphicsCommandList* cl, DescriptorLoadFunc fl, DescriptorHeapType heapType, u32 descriptorSize);
				};
#pragma endregion
			public:
				D3D12DescriptorHeap_Dynamic(D3D12CommandContext& ctrlContext, D3D12_DESCRIPTOR_HEAP_TYPE heapType);

#pragma region Control Methods
				D3D12DescriptorHandle	Allocate(u32 count);
				void					CopyAndBindStagedTables(TableHandleCache& handleCache, ID3D12GraphicsCommandList* cl, DescriptorLoadFunc descriptorLoadFunc);
				void					RetireCurrentHeap();
				void					RefreshStagedBitMap();
				ID3D12DescriptorHeap*	UpdateNativeHeap();
				bool					HasSpace(u32 count);
				void					CleanUpAll(const u64& fenceVal);
#pragma endregion

#pragma region Command Methods
				// @ =========== Pase RS ===========
				inline void ParseRS_Graphics(const D3D12RootSignature& graphicsRS)
				{
					m_GraphicsTableHandleCache.ParseRootSignature(graphicsRS, m_HeapType);
				}
				inline void ParseRS_Compute(const D3D12RootSignature& computeRS)
				{
					m_ComputeTableHandleCache.ParseRootSignature(computeRS, m_HeapType);
				}
				// @ =========== Stage Descriptors ===========
				inline void StageDescriptorHandles_Graphics(u32 rootIndex, u32 offset, u32 numOfHandles, const DescriptorHandle handles[])
				{
					m_GraphicsTableHandleCache.StageDescriptorHandles(rootIndex, offset, numOfHandles, handles);
				}
				inline void StageDescriptorHandles_Compute(u32 rootIndex, u32 offset, u32 numOfHandles, const DescriptorHandle handles[])
				{
					m_ComputeTableHandleCache.StageDescriptorHandles(rootIndex, offset, numOfHandles, handles);
				}

				// @ =========== Commit Descriptors ===========
				inline void CommitDescriptorTables_Graphics(ID3D12GraphicsCommandList* cl)
				{
					if (m_GraphicsTableHandleCache.BitMap_StagedRootParams != 0)
					{
						this->CopyAndBindStagedTables(m_GraphicsTableHandleCache, cl, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
					}
				}
				inline void CommitDescriptorTables_Compute(ID3D12GraphicsCommandList* cl)
				{
					if (m_ComputeTableHandleCache.BitMap_StagedRootParams != 0)
					{
						this->CopyAndBindStagedTables(m_ComputeTableHandleCache, cl, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
					}
				}
#pragma endregion
				
			private:
				static const u32 sm_DefaultHeapSizeInCount = 1024;

				// @ Colletion of heaps at index-0: View
				// @				...    index-1: Sampler
				static std::vector<ID3D12DescriptorHeap*>	sm_DescriptorHeapPools[2];	// actually hold all heaps
				static std::queue<ID3D12DescriptorHeap*>	sm_AvailableDescriptorHeaps[2];
				static std::queue<Pair_FenceValToHeap>		sm_RetiredDescriptorHeaps[2];

				// ** Note that mutex is static, means one at a time, only one of Sampler/View type of DynamicHeap will be able to access these static resource
				static std::mutex							sm_Mutex;		

				static ID3D12DescriptorHeap*	RequestDescriptorHeap (DescriptorHeapType heapType);
				static void						DiscardDescriptorHeaps(DescriptorHeapType heapType, const u64 fenceVal, const std::vector<ID3D12DescriptorHeap*>& heaps);
				static void						DestroyAll();
			private:
				D3D12CommandContext& m_CtrlContext;

#pragma region Fixed Variables
				std::vector<ID3D12DescriptorHeap*>	m_RetiredHeaps;
				const D3D12_DESCRIPTOR_HEAP_TYPE	m_HeapType;		// Sampler OR View
				u32									m_DescriptorSize;

				TableHandleCache					m_GraphicsTableHandleCache;
				TableHandleCache					m_ComputeTableHandleCache;
#pragma endregion

#pragma region Dynamic Variables
				ID3D12DescriptorHeap*	m_CurrentNativeHeap;
				D3D12DescriptorHandle	m_CurrentFirstHandle;
				u32						m_CurrentOffset;
#pragma endregion
			};
		}
	}
}