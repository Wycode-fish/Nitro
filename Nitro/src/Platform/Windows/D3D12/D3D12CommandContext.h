#pragma once
#include "D3D12MemoryAllocator.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12PipelineState.h"
#include "D3D12PixelBuffer.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{

#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
		(D3D12_RESOURCE_STATE_UNORDERED_ACCESS | D3D12_RESOURCE_STATE_COPY_DEST \
			| D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)


#pragma region Forward Desclaration
			class D3D12CommandContext_Graphics;
			class D3D12CommandContext_Compute;
#pragma endregion

			// @ -------------------------------------------------------------------------------------------------------
			// @										Base CommandContext
			// @	* Include:
			// @			- Native Pipeline State
			// @			- Native Root Signatures (1 for compute, 1 for graphics)
			// @			- Native Descriptor Heap Array (1 slot for each type of d3d12 descriptor heap)
			// @			- Native Command List
			// @			- Native Command Allocator (pair with cl)
			// @				+
			// @			- Memory Allocators (Cpu-Writable + GPU-Exclusive)
			// @			- Dynamic Descriptor Heap for Table Root Params (1 for Samplers, 1 for Views)
			// @ -------------------------------------------------------------------------------------------------------
			class D3D12CommandContext
			{
				friend class D3D12CommandContextManager;
				typedef u64 fence_val_t;
				typedef D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_t;

				NON_COPYABLE(D3D12CommandContext);

			public:
				static void DestroyAllContexts();
				static D3D12CommandContext& Begin(const std::wstring id = L"");

				static void	InitializeTexture(D3D12GpuResource& dest, u32 numOfSubres, D3D12_SUBRESOURCE_DATA subresData[]);
				static void InitializeBuffer(D3D12GpuResource& dest, const void* bufferData, const size_t& dataSize, const size_t& destOffset);
				static void ReadbackTexture2D(D3D12GpuResource& readbackBuffer, D3D12PixelBuffer& pixelBuffer);
			public:
#pragma region Command Methods
				void							Initialize();	// Called at the same time it's created inside cmdctx manager.
				fence_val_t						Flush(bool waitForCompletion = false);
				fence_val_t						Finish(bool waitForCompletion = false);
				D3D12CommandContext_Graphics&	AsGraphicsCtx();
				D3D12CommandContext_Compute&	AsComputeCtx();
				void							BindDescriptorHeaps();

				// @ Total 3 Types of D3D12_RESOURCE_BARRIER: https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_resource_barrier
				// @	- Transition	: describe State Transition of a resource.
				// @	- UAV			: mark a resource as it has to finish all current UAV access before starting new UAV access.
				// @	- Aliasing		: ??
				// @
				// @ Total 3 Flags of Barrier: 
				// @	- BEGIN / END / NONE
				void							FlushResourceBarriers();
				void							TransitionResource(D3D12GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImediately = false);
				void							BeginTransitionResource(D3D12GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediately = false);
				void							InsertUAVBarrier(D3D12GpuResource& resource, bool flushImediately = false);

				GpuMemoryBlock					ReserveUploadMemory(const size_t& size);
				void							CopyBuffer(D3D12GpuResource& dest, D3D12GpuResource& src);
				void							CopyBufferRegion(D3D12GpuResource& dest, const size_t& destOffset, D3D12GpuResource& src, const size_t& srcOffset, const size_t& sizeInBytes);
				void							CopySubresource(D3D12GpuResource& dest, u32 destSubIndex, D3D12GpuResource& src, u32 srcSubIndex);
#pragma endregion

#pragma region Setters
				inline void SetPipelineState(const D3D12PipelineState& pso)
				{
					ID3D12PipelineState* native_pso = pso.GetNativePso();
					if (native_pso == m_CurrentPipelineState) 
						return;
					// @ Over-write original PSO
					m_CurrentPipelineState = native_pso;
					m_NativeCommandList->SetPipelineState(m_CurrentPipelineState);
				}
				inline void SetDescriptorHeap(descriptor_heap_t heapType, ID3D12DescriptorHeap* heap)
				{
					// @ Over-write original heap
					if (m_CurrentDescriptorHeapsForAllTypes[heapType] != heap)
					{
						m_CurrentDescriptorHeapsForAllTypes[heapType] = heap;
						this->BindDescriptorHeaps();
					}
				}
				inline void SetDescriptorHeaps(ID3D12DescriptorHeap* heaps, descriptor_heap_t heapTypes[], u32 heapNum)
				{
					bool anyChanged = false;
					for (u32 i = 0; i < heapNum; ++i)
					{
						descriptor_heap_t currentHeapType = heapTypes[i];
						ID3D12DescriptorHeap* currentHeap = heaps + i;
						if (m_CurrentDescriptorHeapsForAllTypes[currentHeapType] != currentHeap)
						{
							m_CurrentDescriptorHeapsForAllTypes[currentHeapType] = currentHeap;
							anyChanged = true;
						}
					}
					if (anyChanged)
					{
						this->BindDescriptorHeaps();
					}
				}
#pragma endregion

			public:
				inline ID3D12GraphicsCommandList* GetCommandList() const { return m_NativeCommandList; }
			public:
				~D3D12CommandContext();

			private:
				D3D12CommandContext(D3D12_COMMAND_LIST_TYPE ccType);
				void Reset();

			protected:
				GpuMemoryAllocator_Linear	m_AllocatorGpuExclusive;
				GpuMemoryAllocator_Linear	m_AllocatorCpuWritable;

				D3D12DescriptorHeap_Dynamic m_CurrentDynamicDescriptorHeap_Sampler;
				D3D12DescriptorHeap_Dynamic m_CurrentDynamicDescriptorHeap_View;

				D3D12_RESOURCE_BARRIER		m_ResourceBarriers[16];
				u32							m_CurrentNumOfBarriersToFlush;

				ID3D12DescriptorHeap*		m_CurrentDescriptorHeapsForAllTypes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
				ID3D12CommandAllocator*		m_CurrentCommandAllocator;
				ID3D12RootSignature*		m_CurrentRootSignature_Graphics;
				ID3D12RootSignature*		m_CurrentRootSignature_Compute;
				ID3D12PipelineState*		m_CurrentPipelineState;
				ID3D12GraphicsCommandList*	m_NativeCommandList;
	
				D3D12_COMMAND_LIST_TYPE		m_ContextType;
#ifdef NT_DEBUG
				std::wstring m_ContextID;
				void SetID(const std::wstring& id) { this->m_ContextID = id; }
#endif
			};

			// @ -------------------------------------------------------------------------------------------------------
			// @										Graphics CommandContext
			// @ -------------------------------------------------------------------------------------------------------
			class D3D12CommandContext_Graphics : public D3D12CommandContext
			{
			public:
				static D3D12CommandContext_Graphics& Begin(const std::wstring& id = L"");
			public:
				void SetRootSignature(const D3D12RootSignature& graphics_rs);
				
				void SetViewPort(float x, float y, float w, float h, float minDepth = 0.f, float maxDepth = 1.f);
				void SetScissor(u32 left, u32 right, u32 top, u32 bottom);
				void SetViewPortAndScissor(u32 x, u32 y, u32 w, u32 h);
				void SetViewPortAndScissor(const D3D12_VIEWPORT& vp, const D3D12_RECT& rect);
				void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY pt);
				
				void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& iboView);
				void SetVertexBuffer(u32 slot, const D3D12_VERTEX_BUFFER_VIEW& vboView);
				void SetVertexBuffers(u32 firstSlot, u32 count, const D3D12_VERTEX_BUFFER_VIEW views[]);
				
				void SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], u32 count);
				void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv);
				void SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], u32 count, D3D12_CPU_DESCRIPTOR_HANDLE dsv);
				void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv);

				void ClearColor(D3D12ColorBuffer& target);
				void ClearColor(const D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float color[]);
				void ClearDepth(D3D12DepthBuffer& depthBuffer);
				void ClearStencil(D3D12DepthBuffer& depthBuffer);
				void ClearDepthAndStencil(D3D12DepthBuffer& depthBuffer);

				void SetDynamicConstantBufferView(u32 rootIndex, const void* bufferData, const size_t& bufferSize);

				void Draw(u32 vertexCount, u32 vertexStartOffset);
				void DrawInstanced(u32 vertexCountPerInstance, u32 instanceCount, u32 firstVertexLocation, u32 firstInstanceLocation);
				void DrawIndexed(u32 indexCount, u32 startIndexLocation, u32 baseVertexLocation);
				void DrawIndexedInstanced(u32 indexCountPerInstance, u32 instanceCount, u32 startIndexLocation, u32 baseVertexLocation, u32 startInstanceLocation);
			};

			// @ -------------------------------------------------------------------------------------------------------
			// @										Compute CommandContext
			// @ -------------------------------------------------------------------------------------------------------
			class D3D12CommandContext_Compute : public D3D12CommandContext
			{
			public:

			private:
			};

			// @ -------------------------------------------------------------------------------------------------------
			// @										CommandContext Manager
			// @ -------------------------------------------------------------------------------------------------------
			class D3D12CommandContextManager
			{
				NON_COPYABLE(D3D12CommandContextManager);
			public:
				static D3D12CommandContextManager*	GetInstance();
				static void							DestroyAllContexts();
				D3D12CommandContext*				AllocateContext(D3D12_COMMAND_LIST_TYPE clType);
				void								FreeContext(D3D12CommandContext* toBeFree);
				void								DestroyContextsInPool();

			private:
				D3D12CommandContextManager();

			private:
				static D3D12CommandContextManager*					sm_Instance;

				std::vector<std::unique_ptr<D3D12CommandContext> >	m_CommandContextPool[4];
				std::queue<D3D12CommandContext*>					m_AvailableCommandContexts[4];
				std::mutex											m_Mutex;
			};
		}
	}
}