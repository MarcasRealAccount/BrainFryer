#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"
#include "Brainfryer/Envs/DX12/DX12GraphicsPipeline.h"
#include "Brainfryer/Envs/DX12/DX12PrimitiveTopology.h"

namespace Brainfryer::DX12
{
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