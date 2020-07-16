#pragma once

#include <d3d12.h>

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

				inline bool IsNull() const { return CpuHandle.ptr == 0; }
				inline bool IsShaderVisible() const { return GpuHandle.ptr != 0; }
#pragma endregion
			};

			// @ ------------------------------------------------------------------
			// @				DescriptorAllocator_CpuViewOnly
			// @ ------------------------------------------------------------------
			struct DescriptorAllocator_CpuViewOnly
			{
				typedef D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
				typedef D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_t;

#pragma region Static Variables
				static std::vector<ID3D12DescriptorHeap*>	s_DescriptorHeapPool;
				static std::mutex							s_AllocationMutex;
				static const u32							s_NumDescriptorPerHeap;
#pragma endregion

#pragma region Static Methods
				static ID3D12DescriptorHeap*	RequestNewHeap(descriptor_heap_t type);
				static void						DestroyPool();
#pragma endregion

#pragma region Member Variables
				ID3D12DescriptorHeap*			CurrentHeap;
				D3D12_CPU_DESCRIPTOR_HANDLE		CurrentHandle;
				D3D12_DESCRIPTOR_HEAP_TYPE		Type;
				u32								DescriptorSize;
				u32								RemainingFreeHandles;
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

			// @ ------------------------------------------------------------------
			// @			DescriptorHeap_Dynamic (automatically request new heap)
			// @ ------------------------------------------------------------------
			class D3D12DescriptorHeap_Dynamic
			{
			public:
			private:
				
			};
		}
	}
}