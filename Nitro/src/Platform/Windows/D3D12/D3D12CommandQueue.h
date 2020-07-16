#pragma once
#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			// @ -----------------------------------------------------
			// @				CommandAllocatorPool
			// @ -----------------------------------------------------
			struct D3D12CommandAllocatorPool
			{
#pragma region Variables
				typedef std::pair<u64, ID3D12CommandAllocator*> FenceVal2AllocatorPair;

				const D3D12_COMMAND_LIST_TYPE			Type;
				std::vector<ID3D12CommandAllocator*>	Pool;
				std::queue<FenceVal2AllocatorPair>		Availables;
				std::mutex								Mutex;
#pragma endregion

#pragma region Control Methods
				ID3D12CommandAllocator* Request(const u64& currentFenceVal);
				void					Discard(const u64& waitingFenceVal, ID3D12CommandAllocator* allocatorExecuted);
				inline void				Shutdown()
				{
					for (auto allocator : Pool)
					{
						NT_DX_RELEASE(allocator);
					}
					Pool.clear();
				}
#pragma endregion

				inline D3D12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type)
					: Type{type}
				{}
				inline ~D3D12CommandAllocatorPool()
				{
					this->Shutdown();
				}

			private:
				ID3D12CommandAllocator* CreateNew() const;
			};

			// @ -----------------------------------------------------
			// @					CommandQueue
			// @ -----------------------------------------------------
			class D3D12CommandQueue
			{
				friend class D3D12CommandQueueManager;
			public:
				typedef u64 fence_val_t;
#pragma region Constructor
				D3D12CommandQueue();
				D3D12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
				~D3D12CommandQueue();
#pragma endregion
			public:
#pragma region Control Methods
				void						Init(ID3D12Device* device);
				fence_val_t					ExecuteCommandLists(ID3D12CommandList* pCmdLists, const u32& uNumOfLists);
				void						CpuWait(const fence_val_t& fenceValToWait);
				void						GpuWait(const fence_val_t& fenceValToWait);
				void						GpuWaitOnOtherQueueComplete(const D3D12CommandQueue& other);
				void						GpuWaitOnOtherQueueToTargetFenceVal(const D3D12CommandQueue& other, const fence_val_t& fenceValToWait);
				void						CpuWaitOnComplete();
				ID3D12CommandAllocator*		RequestAllocator();
				void						DiscardAllocator(const fence_val_t& waitingFenceVal, ID3D12CommandAllocator* allocatorExecuted);
#pragma endregion
				inline ID3D12CommandQueue* GetNativeCommandQueue() const { return m_CommandQueue; }

			private:
				bool		IsFenceValCompleted(const fence_val_t& fenceVal);
				void		UpdateLastFenceCompletedValue();
			
			private:
#pragma region Variables
				D3D12CommandAllocatorPool	m_AllocatorPool;
				u64							m_LastFenceCompletedValue;
				u64							m_NextFenceCompletedValue;

				std::mutex					m_FenceValMutex;
				std::mutex					m_EventMutex;

				ID3D12CommandQueue*			m_CommandQueue;
				ID3D12Fence*				m_Fence;
				HANDLE						m_FenceEvent;
				D3D12_COMMAND_LIST_TYPE		m_CommandQueueType;
#pragma endregion
			};

			// @ -----------------------------------------------------
			// @					CommandQueueManager
			// @ -----------------------------------------------------
			class D3D12CommandQueueManager
			{
				typedef D3D12_COMMAND_LIST_TYPE command_queue_t;
			public:
				static D3D12CommandQueueManager* GetInstance();
				NON_COPYABLE(D3D12CommandQueueManager);
				~D3D12CommandQueueManager();

			public:
				void	Init(ID3D12Device* device);
				void	CpuWait(const D3D12CommandQueue::fence_val_t& fenceValToWait);
				bool	IsFenceComplete(const D3D12CommandQueue::fence_val_t& fenceValToWait);
				void	CreateNewCommandList(
					const command_queue_t& qtype,
					/*out param*/ID3D12GraphicsCommandList** out_pCmdList,
					/*out param*/ID3D12CommandAllocator** out_pAllocator);
				void	CpuWaitAllIdle();

			public:
				inline D3D12CommandQueue* GetCommandQueueByType(const command_queue_t& qtype)
				{
					switch (qtype)
					{
					case D3D12_COMMAND_LIST_TYPE_DIRECT:
						return m_DirectQueue;
					case D3D12_COMMAND_LIST_TYPE_COMPUTE:
						return m_ComputeQueue;
					case D3D12_COMMAND_LIST_TYPE_COPY:
						return m_CopyQueue;
					}
					NT_ASSERT(false, "Command queue type not in use currently. Input was : {0}", Nitro::Util::integral_to_hexstr((u64)qtype));
					return nullptr;
				}
			
			private:
				D3D12CommandQueueManager();
			private:
				static D3D12CommandQueueManager* m_Instance;

			private:
				D3D12CommandQueue* m_DirectQueue;
				D3D12CommandQueue* m_ComputeQueue;
				D3D12CommandQueue* m_CopyQueue;
			};
		}
	}
}