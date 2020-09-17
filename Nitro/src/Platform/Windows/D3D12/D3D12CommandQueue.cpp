#include "NtPCH.h"
#include "D3D12CommandQueue.h"
#include "D3D12Context.h"
#include "Nitro/Application.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			// @ -----------------------------------------------------
			// @				CommandAllocatorPool
			// @ -----------------------------------------------------
			ID3D12CommandAllocator* D3D12CommandAllocatorPool::Request(const u64& currentFenceVal)
			{
				std::lock_guard<std::mutex> internal_lock(this->Mutex);
				ID3D12CommandAllocator* res = nullptr;
				if (!this->Availables.empty())
				{
					FenceVal2AllocatorPair firstAvailable = this->Availables.front();
					if (firstAvailable.first <= currentFenceVal)
					{
						res = firstAvailable.second;
						res->Reset();
						this->Availables.pop();
					}
				}
				if (!res)
				{
					res = this->CreateNew();
					Pool.push_back(res);
				}

				return res;
			}

			void D3D12CommandAllocatorPool::Discard(const u64& waitingFenceVal, ID3D12CommandAllocator* allocatorExecuted)
			{
				std::lock_guard<std::mutex> internal_lock(this->Mutex);
				FenceVal2AllocatorPair pair = std::make_pair(waitingFenceVal, allocatorExecuted);
				this->Availables.push(pair);
			}

			ID3D12CommandAllocator* D3D12CommandAllocatorPool::CreateNew() const
			{
				ID3D12CommandAllocator* res;
				HRESULT hr = Nitro::Graphics::dx::D3D12Context::g_Device->CreateCommandAllocator(Type, IID_PPV_ARGS(&res));
				NT_ASSERT(SUCCEEDED(hr), "New Command Allocator Creation Failed.");
				std::wstring debug_name = NT_STDWSTR_FORMAT("CmdAllocator-{0}", Pool.size());
				res->SetName(debug_name.c_str());
				return res;
			}

			// @ -----------------------------------------------------
			// @					CommandQueue
			// @ -----------------------------------------------------
			D3D12CommandQueue::D3D12CommandQueue()
				: m_AllocatorPool(D3D12_COMMAND_LIST_TYPE_DIRECT)
				, m_LastFenceCompletedValue(0)
				, m_NextFenceCompletedValue(m_LastFenceCompletedValue + 1)
				, m_CommandQueue(nullptr)
				, m_Fence(nullptr)
				, m_FenceEvent(NULL)
				, m_CommandQueueType(D3D12_COMMAND_LIST_TYPE_DIRECT)
			{
			}
			D3D12CommandQueue::D3D12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
				: m_AllocatorPool(type)
				, m_LastFenceCompletedValue(0)
				, m_NextFenceCompletedValue(m_LastFenceCompletedValue + 1)
				, m_CommandQueue(nullptr)
				, m_Fence(nullptr)
				, m_FenceEvent(NULL)
				, m_CommandQueueType(type)
			{
				this->Init(device);
			}
			D3D12CommandQueue::~D3D12CommandQueue()
			{
				CloseHandle(m_FenceEvent);
				NT_DX_RELEASE(m_Fence);
				NT_DX_RELEASE(m_CommandQueue);
			}
			void D3D12CommandQueue::Init(ID3D12Device* device)
			{
				// @ Trick for cmd queue's fence value:
				// @	- Use highest 8-bit for type storage
				// @	- Use lower 56-bit for actual fance value storage.
				this->m_LastFenceCompletedValue = ((u64)m_CommandQueueType) << 56;
				this->m_NextFenceCompletedValue = m_LastFenceCompletedValue + 1;

				D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
				cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				cmdQueueDesc.NodeMask = 0;
				cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				cmdQueueDesc.Type = m_CommandQueueType;
				NT_ASSERT(
					SUCCEEDED(Nitro::Graphics::dx::D3D12Context::g_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CommandQueue))),
					"Command queue creation failed."
				);
				this->m_CommandQueue->SetName(L"D3D12CommandQueue::m_CommandQueue");

				NT_ASSERT(
					SUCCEEDED(Nitro::Graphics::dx::D3D12Context::g_Device->CreateFence(m_LastFenceCompletedValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))),
					"Command queue fence object creation failed."
				);

				m_FenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
				NT_ASSERT(m_FenceEvent != INVALID_HANDLE_VALUE, "Command queue fence event handle creation failed.");
			}

			D3D12CommandQueue::fence_val_t D3D12CommandQueue::ExecuteCommandLists(ID3D12CommandList* pCmdLists, const u32& uNumOfLists)
			{
				// @ Only graphics command list has Close() implemented.
				ID3D12GraphicsCommandList* graphicsCmdLists = static_cast<ID3D12GraphicsCommandList*>(pCmdLists);
				for (u32 i = 0; i < uNumOfLists; ++i)
				{
					HRESULT hr = graphicsCmdLists[i].Close();
					NT_ASSERT(SUCCEEDED(hr), "Command list Close() failed.");
					if (FAILED(hr))
					{
						return -1;
					}
				}
				// @ Actual Execution of cmdlists
				m_CommandQueue->ExecuteCommandLists(uNumOfLists, &pCmdLists);

				// @ Append fence value signal at the end of command queue.
				std::lock_guard<std::mutex> internal_lock(m_FenceValMutex);
				m_CommandQueue->Signal(m_Fence, m_NextFenceCompletedValue);

				return m_NextFenceCompletedValue++;
			}
			void D3D12CommandQueue::CpuWait(const fence_val_t& fenceValToWait)
			{
				if (this->IsFenceValCompleted(fenceValToWait))
				{
					return;
				}
				std::lock_guard<std::mutex> internal_lock(m_EventMutex);
				m_Fence->SetEventOnCompletion(fenceValToWait, m_FenceEvent);
				WaitForSingleObjectEx(m_FenceEvent, INFINITE, false);
				m_LastFenceCompletedValue = fenceValToWait;
			}
			void D3D12CommandQueue::GpuWait(const fence_val_t& fenceValToWait)
			{
				m_CommandQueue->Wait(m_Fence, fenceValToWait);
			}

			void D3D12CommandQueue::GpuWaitOnOtherQueueComplete(const D3D12CommandQueue& other)
			{
				m_CommandQueue->Wait(other.m_Fence, other.m_NextFenceCompletedValue - 1);
			}

			void D3D12CommandQueue::GpuWaitOnOtherQueueToTargetFenceVal(const D3D12CommandQueue& other, const fence_val_t& fenceValToWait)
			{
				m_CommandQueue->Wait(other.m_Fence, fenceValToWait);
			}

			void D3D12CommandQueue::CpuWaitOnComplete()
			{
				this->CpuWait(m_NextFenceCompletedValue - 1);
			}

			ID3D12CommandAllocator* D3D12CommandQueue::RequestAllocator()
			{
				fence_val_t currentFenceVal = m_Fence->GetCompletedValue();
				return m_AllocatorPool.Request(currentFenceVal);
			}

			void D3D12CommandQueue::DiscardAllocator(const fence_val_t& waitingFenceVal, ID3D12CommandAllocator* allocatorExecuted)
			{
				m_AllocatorPool.Discard(waitingFenceVal, allocatorExecuted);
			}

			bool D3D12CommandQueue::IsFenceValCompleted(const fence_val_t& fenceVal)
			{
				if (fenceVal > m_LastFenceCompletedValue)
				{
					this->UpdateLastFenceCompletedValue();
				}
				return fenceVal <= m_LastFenceCompletedValue;
			}

			void D3D12CommandQueue::UpdateLastFenceCompletedValue()
			{
				m_LastFenceCompletedValue = (m_LastFenceCompletedValue >= m_Fence->GetCompletedValue()) ? m_LastFenceCompletedValue : m_Fence->GetCompletedValue();
			}

			// @ -----------------------------------------------------
			// @					CommandQueueManager
			// @ -----------------------------------------------------
			D3D12CommandQueueManager* D3D12CommandQueueManager::m_Instance = nullptr;

			D3D12CommandQueueManager* D3D12CommandQueueManager::GetInstance()
			{
				if (m_Instance == nullptr)
				{
					m_Instance = new D3D12CommandQueueManager();
				}
				return m_Instance;
			}

			D3D12CommandQueueManager::D3D12CommandQueueManager()
				: m_DirectQueue(nullptr)
				, m_ComputeQueue(nullptr)
				, m_CopyQueue(nullptr)
			{
				Init(Nitro::Graphics::dx::D3D12Context::g_Device);
			}

			D3D12CommandQueueManager::~D3D12CommandQueueManager()
			{
				NT_HEAP_RELEASE(m_DirectQueue);
				NT_HEAP_RELEASE(m_ComputeQueue);
				NT_HEAP_RELEASE(m_CopyQueue);
			}
			void D3D12CommandQueueManager::Init(ID3D12Device* device)
			{
				m_DirectQueue = nitro_new D3D12CommandQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
				m_ComputeQueue = nitro_new D3D12CommandQueue(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
				m_CopyQueue = nitro_new D3D12CommandQueue(device, D3D12_COMMAND_LIST_TYPE_COPY);
			}

			void D3D12CommandQueueManager::CpuWait(const D3D12CommandQueue::fence_val_t& fenceValToWait)
			{
				D3D12CommandQueue* targetCommandQueue = GetCommandQueueByType((command_queue_t)(fenceValToWait >> 56));
				targetCommandQueue->CpuWait(fenceValToWait);
			}
			bool D3D12CommandQueueManager::IsFenceComplete(const D3D12CommandQueue::fence_val_t& fenceValToWait)
			{
				D3D12CommandQueue* targetCommandQueue = GetCommandQueueByType((command_queue_t)(fenceValToWait >> 56));
				return targetCommandQueue->IsFenceValCompleted(fenceValToWait);
			}
			void D3D12CommandQueueManager::CreateNewCommandList(const command_queue_t& qtype, ID3D12GraphicsCommandList** out_pCmdList, ID3D12CommandAllocator** out_pAllocator)
			{
				switch (qtype)
				{
				case D3D12_COMMAND_LIST_TYPE_DIRECT:
					*out_pAllocator = m_DirectQueue->RequestAllocator();
					break;
				case D3D12_COMMAND_LIST_TYPE_COMPUTE:
					*out_pAllocator = m_ComputeQueue->RequestAllocator();
					break;
				case D3D12_COMMAND_LIST_TYPE_COPY:
					*out_pAllocator = m_CopyQueue->RequestAllocator();
					break;
				default:
					NT_ASSERT(false, "Unsupported command queue type: {0}", qtype);
					break;
				}
				HRESULT hr = Nitro::Graphics::dx::D3D12Context::g_Device->CreateCommandList(0, qtype, *out_pAllocator, nullptr, IID_PPV_ARGS(out_pCmdList));
				NT_ASSERT(SUCCEEDED(hr), "Command list creation failed. Error: {0}", Nitro::Util::integral_to_hexstr(hr));
			}
			void D3D12CommandQueueManager::CpuWaitAllIdle()
			{
				m_DirectQueue->CpuWaitOnComplete();
				m_ComputeQueue->CpuWaitOnComplete();
				m_CopyQueue->CpuWaitOnComplete();
			}
		}
	}
}