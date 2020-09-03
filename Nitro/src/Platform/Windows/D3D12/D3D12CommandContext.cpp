#include "NtPCH.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandQueue.h"
#include "D3D12Context.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
#pragma region Command Context

			void D3D12CommandContext::BindDescriptorHeaps()
			{
				ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
				u32 numOfHeapsToUpdate = 0;
				for (u32 i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
				{
					if (m_CurrentDescriptorHeapsForAllTypes[i] != nullptr)
					{
						heapsToBind[numOfHeapsToUpdate++] = m_CurrentDescriptorHeapsForAllTypes[i];
					}
				}
				if (numOfHeapsToUpdate > 0)
				{
					m_NativeCommandList->SetDescriptorHeaps(numOfHeapsToUpdate, heapsToBind);
				}
			}

			void D3D12CommandContext::FlushResourceBarriers()
			{
				if (m_CurrentNumOfBarriersToFlush > 0)
				{
					m_NativeCommandList->ResourceBarrier(m_CurrentNumOfBarriersToFlush, m_ResourceBarriers);
					m_CurrentNumOfBarriersToFlush = 0;
				}
			}

			void D3D12CommandContext::TransitionResource(D3D12GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImediately)
			{
				D3D12_RESOURCE_STATES oldState = resource.UsageState;

				// @ If current context is of type COMPUTE, resource state has limit
				if (m_ContextType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
				{
					NT_ASSERT((oldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == oldState, "Invalid resource state in Compute Context.");
					NT_ASSERT((newState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == newState, "Invalid resource state in Compute Context.");
				}

				if (oldState != newState)
				{
					NT_ASSERT(m_CurrentNumOfBarriersToFlush < 16, "Resource barrier num is over limit.");
					D3D12_RESOURCE_BARRIER& barrier = this->m_ResourceBarriers[this->m_CurrentNumOfBarriersToFlush++];
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Transition.pResource = resource.pNativeResource;
					barrier.Transition.StateBefore = oldState;
					barrier.Transition.StateAfter = newState;
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					
					// @ When current resource's already in transitioning:
					// @		- case 1: if we get lucky, it's target state of transition == newState
					// @		- case 2: target state != newState
					if (newState == resource.TransitioningState)
					{
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
						resource.TransitioningState = (D3D12_RESOURCE_STATES)-1;
					}
					else
					{
						// @ Current target state != newState, means resource has begun a half-transition to another state before,
						// @ in this case, we'll first make a full-transition here, without interfering its previoius half-transition record.
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					}

					resource.UsageState = newState;
				}
				else
				{
					if (newState == D3D12_RESOURCE_BARRIER_TYPE_UAV)
					{ 
						this->InsertUAVBarrier(resource, flushImediately);
					}
				}

				if (m_CurrentNumOfBarriersToFlush == 16 || flushImediately)
				{
					this->FlushResourceBarriers();
				}
			}

			void D3D12CommandContext::BeginTransitionResource(D3D12GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediately)
			{
				// @ If already in a half-transition state, we finish it.
				if (resource.TransitioningState != -1)
				{
					this->TransitionResource(resource, newState, flushImmediately);
				}

				D3D12_RESOURCE_STATES oldState = resource.UsageState;
				if (oldState != newState)
				{
					NT_ASSERT(m_CurrentNumOfBarriersToFlush < 16, "Exceeds limit of num on buffered resource barriers.");
					D3D12_RESOURCE_BARRIER& barrier = this->m_ResourceBarriers[this->m_CurrentNumOfBarriersToFlush];
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Transition.pResource = resource.pNativeResource;
					barrier.Transition.StateBefore = oldState;
					barrier.Transition.StateAfter = newState;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;

					resource.TransitioningState = newState;
				}

				if (flushImmediately || m_CurrentNumOfBarriersToFlush == 16)
				{
					this->FlushResourceBarriers();
				}
			}

			void D3D12CommandContext::InsertUAVBarrier(D3D12GpuResource& resource, bool flushImediately)
			{
				NT_ASSERT(m_CurrentNumOfBarriersToFlush < 16, "Exceeds limit of num on buffered resource barriers.");
				D3D12_RESOURCE_BARRIER& barrier = this->m_ResourceBarriers[this->m_CurrentNumOfBarriersToFlush++];

				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.UAV.pResource = resource.pNativeResource;

				if (flushImediately)
				{
					this->FlushResourceBarriers();
				}
			}

			GpuMemoryBlock D3D12CommandContext::ReserveUploadMemory(const size_t& size)
			{
				return this->m_AllocatorCpuWritable.Allocate(size);
			}

			void D3D12CommandContext::CopyBuffer(D3D12GpuResource& dest, D3D12GpuResource& src)
			{
				this->TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
				this->TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
				this->FlushResourceBarriers();
				this->m_NativeCommandList->CopyResource(dest.pNativeResource, src.pNativeResource);
			}

			void D3D12CommandContext::CopyBufferRegion(D3D12GpuResource& dest, const size_t& destOffset, D3D12GpuResource& src, const size_t& srcOffset, const size_t& sizeInBytes)
			{
				this->TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
				this->FlushResourceBarriers();
				this->m_NativeCommandList->CopyBufferRegion(dest.pNativeResource, destOffset, src.pNativeResource, srcOffset, sizeInBytes);
			}

			void D3D12CommandContext::CopySubresource(D3D12GpuResource& dest, u32 destSubIndex, D3D12GpuResource& src, u32 srcSubIndex)
			{
				this->FlushResourceBarriers();
				D3D12_TEXTURE_COPY_LOCATION destLocation = { dest.pNativeResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, destSubIndex };
				D3D12_TEXTURE_COPY_LOCATION srcLocation = { src.pNativeResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, srcSubIndex };
				this->m_NativeCommandList->CopyTextureRegion(
					&destLocation, 0, 0, 0,
					&srcLocation, nullptr
				);
			}

			void D3D12CommandContext::InitializeTexture(D3D12GpuResource& dest, u32 numOfSubres, D3D12_SUBRESOURCE_DATA subresData[])
			{
				D3D12CommandContext& ctx = D3D12CommandContext::Begin();
				
				u64 uploadBufferSize = Util::DX::GetRequiredIntermediateSize(dest.pNativeResource, 0, numOfSubres);
				GpuMemoryBlock memblk = ctx.ReserveUploadMemory(uploadBufferSize);
				u64 res = Util::DX::UpdateSubresources(ctx.GetCommandList(), dest.pNativeResource, memblk.ContextBuffer.pNativeResource, 0, numOfSubres, 0, subresData);
				ctx.TransitionResource(dest, D3D12_RESOURCE_STATE_GENERIC_READ);

				ctx.Finish(true);
			}

			void D3D12CommandContext::InitializeBuffer(D3D12GpuResource& dest, const void* bufferData, const size_t& dataSize, const size_t& destOffset)
			{
				D3D12CommandContext& ctx = D3D12CommandContext::Begin();

				GpuMemoryBlock memblk = ctx.ReserveUploadMemory(dataSize);
				Util::simd_memcpy(memblk.CpuAddress, bufferData, Util::divide_1base(dataSize, 16));

				ctx.TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST, true);
				ctx.GetCommandList()->CopyBufferRegion(dest.pNativeResource, destOffset, memblk.ContextBuffer.pNativeResource, 0, dataSize);
				ctx.TransitionResource(dest, D3D12_RESOURCE_STATE_GENERIC_READ, true);

				ctx.Finish(true);
			}

			void D3D12CommandContext::ReadbackTexture2D(D3D12GpuResource& readbackBuffer, D3D12PixelBuffer& pixelBuffer)
			{
				D3D12CommandContext& ctx = D3D12CommandContext::Begin(L"Copy texture to buffer");
				ctx.TransitionResource(pixelBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, true);
				
				D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
				D3D12Context::g_Device->GetCopyableFootprints(
					&pixelBuffer.pNativeResource->GetDesc(), 
					0/*1st subresource*/, 1/* only get footprint of 1 subresource*/, 0,
					&placedFootprint, nullptr, nullptr, nullptr
				);
				
				ctx.GetCommandList()->CopyTextureRegion(
					&Util::DX::TEXTURE_COPY_LOCATION(readbackBuffer.pNativeResource, placedFootprint), 0, 0, 0,
					&Util::DX::TEXTURE_COPY_LOCATION(pixelBuffer.pNativeResource, 0), nullptr);
				
				ctx.Finish(true);
			}

			D3D12CommandContext::D3D12CommandContext(D3D12_COMMAND_LIST_TYPE ccType)
				: m_AllocatorGpuExclusive(PageType::GpuExclusive)
				, m_AllocatorCpuWritable(PageType::CpuWritable)
				, m_CurrentDynamicDescriptorHeap_Sampler(*this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
				, m_CurrentDynamicDescriptorHeap_View(*this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				, m_CurrentNumOfBarriersToFlush(0)
				, m_CurrentCommandAllocator(nullptr)
				, m_CurrentRootSignature_Graphics(nullptr)
				, m_CurrentRootSignature_Compute(nullptr)
				, m_CurrentPipelineState(nullptr)
				, m_NativeCommandList(nullptr)
				, m_ContextType(ccType)
			{
				memset(this->m_CurrentDescriptorHeapsForAllTypes, 0, sizeof(this->m_CurrentDescriptorHeapsForAllTypes));
			}

			D3D12CommandContext::~D3D12CommandContext()
			{
				NT_DX_RELEASE(this->m_NativeCommandList);
			}

			void D3D12CommandContext::Reset()
			{
				NT_ASSERT(m_NativeCommandList != nullptr && m_CurrentCommandAllocator == nullptr, "Context hasn't get initialized at the first place");
				m_CurrentCommandAllocator = D3D12CommandQueueManager::GetInstance()->GetCommandQueueByType(m_ContextType)->RequestAllocator();
				m_NativeCommandList->Reset(m_CurrentCommandAllocator, nullptr);

				m_CurrentRootSignature_Graphics = nullptr;
				m_CurrentRootSignature_Compute = nullptr;
				m_CurrentPipelineState = nullptr;
				m_CurrentNumOfBarriersToFlush = 0;

				this->BindDescriptorHeaps();
			}

			void D3D12CommandContext::DestroyAllContexts()
			{
				GpuMemoryAllocator_Linear::DetroyAll();
				D3D12DescriptorHeap_Dynamic::DestroyAll();
				D3D12CommandContextManager::DestroyAllContexts();
			}

			D3D12CommandContext& D3D12CommandContext::Begin(const std::wstring id)
			{
				D3D12CommandContext* newContext = D3D12CommandContextManager::GetInstance()->AllocateContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
#ifdef NT_DEBUG
				newContext->SetID(id);
				/* Performance Profiling stuff goes here...*/
#endif
				return *newContext;
			}

			void D3D12CommandContext::Initialize()
			{
				D3D12CommandQueueManager::GetInstance()->CreateNewCommandList(this->m_ContextType, /*out_param*/&this->m_NativeCommandList, /*out_param*/&this->m_CurrentCommandAllocator);
			}

			D3D12CommandContext::fence_val_t D3D12CommandContext::Flush(bool waitForCompletion)
			{
				this->FlushResourceBarriers();

				// @ Actual execution of all recorded commands
				NT_ASSERT(this->m_CurrentCommandAllocator != nullptr, "Current command allocator for context is null.");
				fence_val_t finishFenceVal = D3D12CommandQueueManager::GetInstance()->GetCommandQueueByType(this->m_ContextType)->ExecuteCommandLists(m_NativeCommandList, 1);

				// @ Reset command list and retore previous state
				if (waitForCompletion)
				{
					D3D12CommandQueueManager::GetInstance()->CpuWait(finishFenceVal);
				}

				this->m_NativeCommandList->Reset(m_CurrentCommandAllocator, nullptr);
				if (this->m_CurrentRootSignature_Graphics != nullptr)
				{
					this->m_NativeCommandList->SetGraphicsRootSignature(m_CurrentRootSignature_Graphics);
				}
				if (this->m_CurrentRootSignature_Compute != nullptr)
				{
					this->m_NativeCommandList->SetGraphicsRootSignature(m_CurrentRootSignature_Compute);
				}
				if (this->m_CurrentPipelineState != nullptr)
				{
					this->m_NativeCommandList->SetPipelineState(m_CurrentPipelineState);
				}
				this->BindDescriptorHeaps();

				return finishFenceVal;
			}

			D3D12CommandContext::fence_val_t D3D12CommandContext::Finish(bool waitForCompletion)
			{
				NT_ASSERT(
					m_ContextType == D3D12_COMMAND_LIST_TYPE_DIRECT || m_ContextType == D3D12_COMMAND_LIST_TYPE_COMPUTE,
					"Invalid command list type for CommandContext."
				);

				this->FlushResourceBarriers();
				/* Performance Profiling stuff goes here...*/

				NT_ASSERT(m_CurrentCommandAllocator != nullptr, "Current command allocator is null.");
				D3D12CommandQueue* targetCmdQueue = D3D12CommandQueueManager::GetInstance()->GetCommandQueueByType(this->m_ContextType);
				fence_val_t finishFenceVal = targetCmdQueue->ExecuteCommandLists(m_NativeCommandList, 1);

				targetCmdQueue->DiscardAllocator(finishFenceVal, m_CurrentCommandAllocator);
				m_CurrentCommandAllocator = nullptr;

				m_AllocatorCpuWritable.CleanupUsedPages(finishFenceVal);
				m_AllocatorGpuExclusive.CleanupUsedPages(finishFenceVal);
				m_CurrentDynamicDescriptorHeap_View.CleanUpAll(finishFenceVal);
				m_CurrentDynamicDescriptorHeap_Sampler.CleanUpAll(finishFenceVal);

				if (waitForCompletion)
				{
					targetCmdQueue->CpuWait(finishFenceVal);
				}
				D3D12CommandContextManager::GetInstance()->FreeContext(this);
				return finishFenceVal;
			}

			D3D12CommandContext_Graphics& D3D12CommandContext::AsGraphicsCtx()
			{
				return reinterpret_cast<D3D12CommandContext_Graphics&>(*this);
			}

			D3D12CommandContext_Compute& D3D12CommandContext::AsComputeCtx()
			{
				return reinterpret_cast<D3D12CommandContext_Compute&>(*this);
			}
#pragma endregion

#pragma region CommandContext Manager
			D3D12CommandContextManager* D3D12CommandContextManager::sm_Instance = nullptr;
			D3D12CommandContextManager* D3D12CommandContextManager::GetInstance()
			{
				if (sm_Instance == nullptr)
				{
					sm_Instance = nitro_new D3D12CommandContextManager();
				}
				return sm_Instance;
			}
			
			D3D12CommandContextManager::D3D12CommandContextManager()
			{}

			void D3D12CommandContextManager::DestroyAllContexts()
			{
				NT_ASSERT(D3D12CommandContextManager::GetInstance() != nullptr, "CommandContext Manager has not yet been created.");
				D3D12CommandContextManager::GetInstance()->DestroyContextsInPool();
			}

			D3D12CommandContext* D3D12CommandContextManager::AllocateContext(D3D12_COMMAND_LIST_TYPE clType)
			{
				std::lock_guard<std::mutex> internal_lock(this->m_Mutex);

				std::queue<D3D12CommandContext*>& availableCommandContexts = m_AvailableCommandContexts[clType];
				std::vector<std::unique_ptr<D3D12CommandContext> >& commandContextPool = m_CommandContextPool[clType];

				D3D12CommandContext* res;
				if (availableCommandContexts.empty())
				{
					res = new D3D12CommandContext(clType);
					commandContextPool.emplace_back(res);
					res->Initialize();
				}
				else
				{
					res = availableCommandContexts.front();
					availableCommandContexts.pop();
					res->Reset();
				}

				NT_ASSERT(res != nullptr, "Creation of command context failed.");

				return res;
			}

			void D3D12CommandContextManager::FreeContext(D3D12CommandContext* toBeFree)
			{
				NT_ASSERT(toBeFree != nullptr, "Can't free a null context.");
				std::lock_guard<std::mutex> internal_lock(m_Mutex);
				m_AvailableCommandContexts[toBeFree->m_ContextType].push(toBeFree);
			}

			void D3D12CommandContextManager::DestroyContextsInPool()
			{
				// @ No need to do NT_DX_RELEASE() here, as contexts're stored as unique_ptrs
				m_CommandContextPool->clear();
			}
#pragma endregion

#pragma region Graphics CommandContext

			D3D12CommandContext_Graphics& D3D12CommandContext_Graphics::Begin(const std::wstring& id)
			{
				return D3D12CommandContext::Begin(id).AsGraphicsCtx();
			}

			void D3D12CommandContext_Graphics::SetRootSignature(const D3D12RootSignature& graphics_rs)
			{
				if (graphics_rs.GetNativeRootSignature() == this->m_CurrentRootSignature_Graphics)
				{
					return;
				}
				this->m_CurrentRootSignature_Graphics = graphics_rs.GetNativeRootSignature();
				this->m_NativeCommandList->SetGraphicsRootSignature(m_CurrentRootSignature_Graphics);
				this->m_CurrentDynamicDescriptorHeap_Sampler.ParseRS_Graphics(graphics_rs);
				this->m_CurrentDynamicDescriptorHeap_View.ParseRS_Graphics(graphics_rs);
			}

			void D3D12CommandContext_Graphics::SetViewPort(float x, float y, float w, float h, float minDepth, float maxDepth)
			{
				D3D12_VIEWPORT view_port = {};
				view_port.Height = h;
				view_port.Width = w;
				view_port.TopLeftX = x;
				view_port.TopLeftY = y;
				view_port.MinDepth = minDepth;
				view_port.MaxDepth = maxDepth;
				this->m_NativeCommandList->RSSetViewports(1, &view_port);
			}

			void D3D12CommandContext_Graphics::SetScissor(u32 left, u32 right, u32 top, u32 bottom)
			{
				D3D12_RECT scissor_rect = {};
				scissor_rect.left = left;
				scissor_rect.top = top;
				scissor_rect.right = right;
				scissor_rect.bottom = bottom;
				this->m_NativeCommandList->RSSetScissorRects(1, &scissor_rect);
			}

			void D3D12CommandContext_Graphics::SetViewPortAndScissor(u32 x, u32 y, u32 w, u32 h)
			{
				this->SetViewPort(x, y, w, h);
				this->SetScissor(x, x + w, y, y + h);
			}

			void D3D12CommandContext_Graphics::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY pt)
			{
				this->m_NativeCommandList->IASetPrimitiveTopology(pt);
			}

			void D3D12CommandContext_Graphics::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& iboView)
			{
				this->m_NativeCommandList->IASetIndexBuffer(&iboView);
			}

			void D3D12CommandContext_Graphics::SetVertexBuffer(u32 slot, const D3D12_VERTEX_BUFFER_VIEW& vboView)
			{
				this->SetVertexBuffers(slot, 1, &vboView);
			}

			void D3D12CommandContext_Graphics::SetVertexBuffers(u32 firstSlot, u32 count, const D3D12_VERTEX_BUFFER_VIEW views[])
			{
				this->m_NativeCommandList->IASetVertexBuffers(firstSlot, count, views);
			}

			void D3D12CommandContext_Graphics::SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], u32 count)
			{
				this->m_NativeCommandList->OMSetRenderTargets(count, rtvs, FALSE, nullptr);
			}

			void D3D12CommandContext_Graphics::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv)
			{
				this->SetRenderTargets(&rtv, 1);
			}

			void D3D12CommandContext_Graphics::SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], u32 count, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
			{
				this->m_NativeCommandList->OMSetRenderTargets(count, rtvs, FALSE, &dsv);
			}

			void D3D12CommandContext_Graphics::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
			{
				this->SetRenderTargets(&rtv, 1, dsv);
			}

			void D3D12CommandContext_Graphics::ClearColor(D3D12ColorBuffer& target)
			{
				this->m_NativeCommandList->ClearRenderTargetView(target.RTV, target.ClearColor.GetPtr(), 0, nullptr);
			}

			void D3D12CommandContext_Graphics::ClearColor(const D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float color[])
			{
				this->m_NativeCommandList->ClearRenderTargetView(rtv, color, 0, nullptr);
			}

			void D3D12CommandContext_Graphics::ClearDepth(D3D12DepthBuffer& depthBuffer)
			{
				this->m_NativeCommandList->ClearDepthStencilView(
					depthBuffer.GetDSV(D3D12DepthBuffer::DSV_ReadWrite), 
					D3D12_CLEAR_FLAG_DEPTH, 
					depthBuffer.ClearDepth, depthBuffer.ClearStencil, 
					0, nullptr);
			}

			void D3D12CommandContext_Graphics::ClearStencil(D3D12DepthBuffer& depthBuffer)
			{
				this->m_NativeCommandList->ClearDepthStencilView(
					depthBuffer.GetDSV(D3D12DepthBuffer::DSV_ReadWrite),
					D3D12_CLEAR_FLAG_STENCIL,
					depthBuffer.ClearDepth, depthBuffer.ClearStencil,
					0, nullptr);
			}

			void D3D12CommandContext_Graphics::ClearDepthAndStencil(D3D12DepthBuffer& depthBuffer)
			{
				this->m_NativeCommandList->ClearDepthStencilView(
					depthBuffer.GetDSV(D3D12DepthBuffer::DSV_ReadWrite),
					D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
					depthBuffer.ClearDepth, depthBuffer.ClearStencil,
					0, nullptr);
			}

			void D3D12CommandContext_Graphics::SetDynamicConstantBufferView(u32 rootIndex, const void* bufferData, const size_t& bufferSize)
			{
				NT_ASSERT(bufferData != nullptr && bufferSize > 0 && Util::isAligned(bufferData, DX_DATA_ALIGN_DEFAULT), "Invalid data for CBO");
				GpuMemoryBlock mem = this->m_AllocatorCpuWritable.Allocate(bufferSize);
				::memcpy(mem.CpuAddress, bufferData, bufferSize);
				this->m_NativeCommandList->SetGraphicsRootConstantBufferView(rootIndex, mem.GpuAddress);
			}

			void D3D12CommandContext_Graphics::Draw(u32 vertexCount, u32 vertexStartOffset)
			{
				this->DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
			}

			void D3D12CommandContext_Graphics::DrawInstanced(u32 vertexCountPerInstance, u32 instanceCount, u32 firstVertexLocation, u32 firstInstanceLocation)
			{
				this->FlushResourceBarriers();

				this->m_CurrentDynamicDescriptorHeap_View.CommitDescriptorTables_Graphics(this->m_NativeCommandList);
				this->m_CurrentDynamicDescriptorHeap_Sampler.CommitDescriptorTables_Compute(this->m_NativeCommandList);

				this->m_NativeCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, firstVertexLocation, firstInstanceLocation);
			}
			void D3D12CommandContext_Graphics::DrawIndexed(u32 indexCount, u32 startIndexLocation, u32 baseVertexLocation)
			{
				this->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
			}
			void D3D12CommandContext_Graphics::DrawIndexedInstanced(u32 indexCountPerInstance, u32 instanceCount, u32 startIndexLocation, u32 baseVertexLocation, u32 startInstanceLocation)
			{
				this->FlushResourceBarriers();
				this->m_CurrentDynamicDescriptorHeap_View.CommitDescriptorTables_Graphics(this->m_NativeCommandList);
				this->m_CurrentDynamicDescriptorHeap_Sampler.CommitDescriptorTables_Graphics(this->m_NativeCommandList);
				this->m_NativeCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
			}
#pragma endregion
		}
	}
}