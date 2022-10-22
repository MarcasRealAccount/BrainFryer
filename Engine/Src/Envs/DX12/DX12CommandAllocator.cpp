#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"

namespace Brainfryer::DX12
{
	DX12CommandAllocator::DX12CommandAllocator(const CommandAllocatorInfo& info)
	    : m_Type(info.type)
	{
		auto context = Context::Get<DX12Context>();
		HRVLT(context->device()->CreateCommandAllocator(DX12CommandListType(m_Type), m_Allocator, m_Allocator));
	}

	DX12CommandAllocator::DX12CommandAllocator(ID3D12Device9* device, ECommandListType type)
	    : m_Type(type)
	{
		HRVLT(device->CreateCommandAllocator(DX12CommandListType(m_Type), m_Allocator, m_Allocator));
	}

	DX12CommandAllocator::DX12CommandAllocator(DX12CommandAllocator&& move) noexcept
	    : m_Type(move.m_Type),
	      m_Allocator(std::move(move.m_Allocator))
	{
	}

	DX12CommandAllocator::~DX12CommandAllocator()
	{
	}

	void DX12CommandAllocator::reset()
	{
		HRVLT(m_Allocator->Reset());
	}

	std::vector<std::unique_ptr<CommandList>> DX12CommandAllocator::allocate(std::size_t numCommandLists)
	{
		std::vector<std::unique_ptr<CommandList>> list(numCommandLists);
		for (std::size_t i = 0; i < numCommandLists; ++i)
			list.emplace_back(std::make_unique<DX12CommandList>(this));
		return list;
	}

	std::vector<std::unique_ptr<CommandList>> DX12CommandAllocator::allocate(ID3D12Device9* device, std::size_t numCommandLists)
	{
		std::vector<std::unique_ptr<CommandList>> list(numCommandLists);
		for (std::size_t i = 0; i < numCommandLists; ++i)
			list.emplace_back(std::make_unique<DX12CommandList>(this, device));
		return list;
	}
} // namespace Brainfryer::DX12