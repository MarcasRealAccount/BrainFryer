#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"

namespace Brainfryer::DX12
{
	DX12CommandList::DX12CommandList(DX12CommandAllocator* allocator)
	    : m_Allocator(allocator)
	{
		auto  context = Context::Get<DX12Context>();
		auto& handle  = allocator->handle();

		if (!HRValidate(context->device()->CreateCommandList(0, DX12CommandListType(allocator->type()), handle.get(), nullptr, m_CommandList, m_CommandList)))
			return;

		if (!HRValidate(m_CommandList->Close()))
			return;
	}

	DX12CommandList::DX12CommandList(DX12CommandAllocator* allocator, ID3D12Device10* device)
	    : m_Allocator(allocator)
	{
		if (!HRValidate(device->CreateCommandList(0, DX12CommandListType(allocator->type()), allocator->handle().get(), nullptr, m_CommandList, m_CommandList)))
			return;

		if (!HRValidate(m_CommandList->Close()))
			return;
	}

	DX12CommandList::DX12CommandList(DX12CommandList&& move) noexcept
	    : m_Allocator(move.m_Allocator),
	      m_CommandList(std::move(move.m_CommandList))
	{
	}

	DX12CommandList::~DX12CommandList()
	{
	}

	void DX12CommandList::begin()
	{
		if (!HRValidate(m_CommandList->Reset(m_Allocator->handle().get(), nullptr)))
			return;
	}

	void DX12CommandList::end()
	{
		if (!HRValidate(m_CommandList->Close()))
			return;
	}

	ECommandListType DX12CommandList::type() const
	{
		return m_Allocator->type();
	}
} // namespace Brainfryer::DX12