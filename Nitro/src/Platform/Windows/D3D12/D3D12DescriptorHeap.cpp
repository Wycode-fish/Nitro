#include "NtPCH.h"
#include "D3D12DescriptorHeap.h"
#include "Nitro/Application.h"
#include "Platform/Windows/D3D12/D3D12Context.h"

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
				CpuHandle.ptr = 0;
				GpuHandle.ptr = 0;
			}
			D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu)
				: CpuHandle(hCpu)
			{
				GpuHandle.ptr = 0;
			}
			D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hCpu, D3D12_GPU_DESCRIPTOR_HANDLE hGpu)
				: CpuHandle(hCpu), GpuHandle(hGpu)
			{
			}

			D3D12DescriptorHandle& D3D12DescriptorHandle::operator+=(u32 offsetInByte)
			{
				if (this->CpuHandle.ptr)
					this->CpuHandle.ptr += offsetInByte;
				if (this->GpuHandle.ptr)
					this->GpuHandle.ptr += offsetInByte;
				return *this;
			}

			D3D12DescriptorHandle D3D12DescriptorHandle::operator+(u32 offsetInByte) const
			{
				D3D12DescriptorHandle res = *this;
				res += offsetInByte;
				return res;
			}

			// @ ------------------------------------------------------------------
			// @				DescriptorAllocator_CpuViewOnly
			// @ ------------------------------------------------------------------
			std::vector<ID3D12DescriptorHeap*>	DescriptorAllocator_CpuViewOnly::s_DescriptorHeapPool;
			std::mutex							DescriptorAllocator_CpuViewOnly::s_AllocationMutex;
			const u32							DescriptorAllocator_CpuViewOnly::s_NumDescriptorPerHeap = BIT(8);

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
				s_DescriptorHeapPool.clear();
			}

			DescriptorAllocator_CpuViewOnly::DescriptorAllocator_CpuViewOnly(descriptor_heap_t type)
				: Type(type)
				, CurrentHeap(nullptr)
				, RemainingFreeHandles(0)
				, DescriptorSize(0)
			{
				CurrentHandle.ptr = 0;
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
			{
			}

			void D3D12DescriptorHeap_Static::Init(const std::string& name, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				// @ ==== Descriptor Heap Flags
				// @		- The flag D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE can optionally be set on a descriptor heap to indicate it is be bound on a command list for reference by shaders.Descriptor heaps created without this flag allow applications the option to stage descriptors in CPU memory before copying them to a shader visible descriptor heap, as a convenience.But it is also fine for applications to directly create descriptors into shader visible descriptor heaps with no requirement to stage anything on the CPU.
				// @		- This flag only applies to CBV, SRV, UAVand samplers.It does not apply to other descriptor heap types since shaders do not directly reference the other types.
				NT_ASSERT(
					((m_Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) && (flags != D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)) || (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
					"Flag D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE only applies to CBV, SRV, UAVand samplers."
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
				this->m_FirstHandle = m_Heap->GetCPUDescriptorHandleForHeapStart();
				this->m_DescriptorSize = D3D12Context::g_Device->GetDescriptorHandleIncrementSize(this->m_Type);

			}

			bool D3D12DescriptorHeap_Static::HasAvailableSpace(u32 requestNum)
			{
				return false;
			}

			D3D12DescriptorHandle D3D12DescriptorHeap_Static::Allocate(u32 requestNum)
			{
				return D3D12DescriptorHandle();
			}

			bool D3D12DescriptorHeap_Static::ValidateHandle(const D3D12DescriptorHandle& descriptorHandle) const
			{
				return false;
			}

		}
	}
}