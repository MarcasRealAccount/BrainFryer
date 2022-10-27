#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12DescriptorHeap.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"
#include "Brainfryer/Envs/DX12/DX12GraphicsPipeline.h"
#include "Brainfryer/Envs/DX12/DX12Image.h"
#include "Brainfryer/Envs/DX12/DX12PrimitiveTopology.h"
#include "Brainfryer/Envs/DX12/DX12RenderTargets.h"

namespace Brainfryer::DX12
{
	DX12CommandList::DX12CommandList()
	    : m_Allocator(nullptr) {}

	DX12CommandList::DX12CommandList(DX12CommandAllocator* allocator)
	    : m_Allocator(allocator)
	{
		auto  context = Context::Get<DX12Context>();
		auto& handle  = allocator->handle();

		HRVLT(context->device()->CreateCommandList(0, DX12CommandListType(allocator->type()), handle.get(), nullptr, m_CommandList, m_CommandList));
		HRVLT(m_CommandList->Close());
	}

	DX12CommandList::DX12CommandList(DX12CommandAllocator* allocator, ID3D12Device9* device)
	    : m_Allocator(allocator)
	{
		HRVLT(device->CreateCommandList(0, DX12CommandListType(allocator->type()), allocator->handle().get(), nullptr, m_CommandList, m_CommandList));
		HRVLT(m_CommandList->Close());
	}

	DX12CommandList::DX12CommandList(DX12CommandList&& move) noexcept
	    : m_Allocator(move.m_Allocator),
	      m_CommandList(std::move(move.m_CommandList))
	{
	}

	DX12CommandList::~DX12CommandList()
	{
	}

	void DX12CommandList::begin(GraphicsPipeline* initialPipeline)
	{
		HRVLT(m_CommandList->Reset(m_Allocator->handle().get(), initialPipeline ? static_cast<DX12GraphicsPipeline*>(initialPipeline)->handle().get() : nullptr));
		if (initialPipeline)
			m_CommandList->SetGraphicsRootSignature(static_cast<DX12GraphicsPipeline*>(initialPipeline)->rootSignature().get());
	}

	void DX12CommandList::end()
	{
		HRVLT(m_CommandList->Close());
	}

	void DX12CommandList::bindRenderTargets(RenderTargets* renderTargets, std::uint32_t index)
	{
		auto dx12RenderTargets = static_cast<DX12RenderTargets*>(renderTargets);
		auto rtvStart          = dx12RenderTargets->getRTVStart(index);
		auto dsvStart          = dx12RenderTargets->getDSVStart(index);
		m_CommandList->OMSetRenderTargets(renderTargets->colorCount(), &rtvStart, true, renderTargets->hasDepthStencil() ? &dsvStart : nullptr);

		std::uint32_t i = 0;
		for (auto& color : dx12RenderTargets->colors())
		{
			auto& clear = static_cast<DX12FrameImage*>(color.image)->clearValue();
			if (clear.color[0] < 0)
			{
				++i;
				continue;
			}

			auto rtv = dx12RenderTargets->getRTV(i, index);
			m_CommandList->ClearRenderTargetView(rtv, clear.color, 0, nullptr);
			++i;
		}

		if (renderTargets->hasDepthStencil())
		{
			auto&         depthStencil = dx12RenderTargets->depthStencil();
			auto&         clear        = static_cast<DX12FrameImage*>(depthStencil.image)->clearValue();
			std::uint32_t flags        = 0;
			if (clear.ds.depth >= 0.0f)
				flags |= D3D12_CLEAR_FLAG_DEPTH;
			if (clear.ds.stencil >= 0)
				flags |= D3D12_CLEAR_FLAG_STENCIL;

			if (flags != 0)
				m_CommandList->ClearDepthStencilView(dx12RenderTargets->getDSVStart(index), static_cast<D3D12_CLEAR_FLAGS>(flags), clear.ds.depth, static_cast<std::uint8_t>(clear.ds.stencil), 0, nullptr);
		}
	}

	void DX12CommandList::setViewports(const std::vector<Viewport>& viewports)
	{
		std::vector<D3D12_VIEWPORT> vps(viewports.size());
		for (std::size_t i = 0; i < vps.size(); ++i)
		{
			auto& viewport = viewports[i];
			vps[i]         = { viewport.x, viewport.y, viewport.w, viewport.h, viewport.minDepth, viewport.maxDepth };
		}
		m_CommandList->RSSetViewports(static_cast<std::uint32_t>(vps.size()), vps.data());
	}

	void DX12CommandList::setScissors(const std::vector<Rect>& scissors)
	{
		std::vector<D3D12_RECT> scs(scissors.size());
		for (std::size_t i = 0; i < scs.size(); ++i)
		{
			auto& scissor = scissors[i];
			scs[i]        = { scissor.x, scissor.y, static_cast<std::int32_t>(scissor.w), static_cast<std::int32_t>(scissor.h) };
		}
		m_CommandList->RSSetScissorRects(static_cast<std::uint32_t>(scs.size()), scs.data());
	}

	void DX12CommandList::setDescriptorHeaps(const std::vector<DescriptorHeap*>& heaps)
	{
		std::vector<ID3D12DescriptorHeap*> hps(heaps.size());
		for (std::size_t i = 0; i < hps.size(); ++i)
			hps[i] = static_cast<DX12DescriptorHeap*>(heaps[i])->handle().get();
		m_CommandList->SetDescriptorHeaps(static_cast<UINT>(hps.size()), hps.data());
	}

	void DX12CommandList::bindDescriptorTable(std::uint32_t binding, DescriptorHeapRef heapRef)
	{
		auto handle = static_cast<DX12DescriptorHeap*>(heapRef.heap())->descriptorHandle(heapRef.index());
		m_CommandList->SetGraphicsRootDescriptorTable(binding, handle);
	}

	void DX12CommandList::bind32BitConstants(std::uint32_t binding, std::uint32_t count, const void* data, std::uint32_t offset)
	{
		m_CommandList->SetGraphicsRoot32BitConstants(binding, count, data, offset);
	}

	void DX12CommandList::setBlendFactor(const float (&factor)[4])
	{
		m_CommandList->OMSetBlendFactor(factor);
	}

	void DX12CommandList::setPrimitiveTopology(EPrimitiveTopology topology)
	{
		m_CommandList->IASetPrimitiveTopology(DX12PrimitiveTopology(topology));
	}

	void DX12CommandList::setVertexBuffers(std::uint32_t startIndex, const std::vector<VertexBufferView>& buffers)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> views(buffers.size());
		for (std::size_t i = 0; i < buffers.size(); ++i)
		{
			auto& buffer = buffers[i];
			auto& view   = views[i];

			view.BufferLocation = static_cast<DX12Buffer*>(buffer.buffer)->handle()->GetGPUVirtualAddress() + buffer.offset;
			view.SizeInBytes    = buffer.size;
			view.StrideInBytes  = buffer.stride;
		}
		m_CommandList->IASetVertexBuffers(startIndex, static_cast<UINT>(views.size()), views.data());
	}

	void DX12CommandList::setIndexBuffer(IndexBufferView indexBuffer)
	{
		D3D12_INDEX_BUFFER_VIEW view {};
		view.BufferLocation = static_cast<DX12Buffer*>(indexBuffer.buffer)->handle()->GetGPUVirtualAddress() + indexBuffer.offset;
		view.SizeInBytes    = indexBuffer.size;
		view.Format         = DX12Format(indexBuffer.format);
		m_CommandList->IASetIndexBuffer(&view);
	}

	void DX12CommandList::drawInstanced(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t startVertex, std::uint32_t startInstance)
	{
		m_CommandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void DX12CommandList::drawIndexedInstanced(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t startIndex, std::uint32_t startVertex, std::uint32_t startInstance)
	{
		m_CommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, startVertex, startInstance);
	}

	ECommandListType DX12CommandList::type() const
	{
		return m_Allocator->type();
	}
} // namespace Brainfryer::DX12