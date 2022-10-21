#pragma once

#include "Brainfryer/Renderer/CommandAllocator.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	class DX12CommandAllocator : public CommandAllocator
	{
	public:
		DX12CommandAllocator(const CommandAllocatorInfo& info);
		DX12CommandAllocator(ID3D12Device10* device, ECommandListType type);
		DX12CommandAllocator(DX12CommandAllocator&& move) noexcept;
		~DX12CommandAllocator();

		virtual void reset() override;

		virtual std::vector<std::unique_ptr<CommandList>> allocate(std::size_t numCommandLists) override;
		std::vector<std::unique_ptr<CommandList>>         allocate(ID3D12Device10* device, std::size_t numCommandLists);

		virtual ECommandListType type() const override { return m_Type; }

		virtual bool initialized() const { return m_Allocator.valid(); }

		auto& handle() { return m_Allocator; }
		auto& handle() const { return m_Allocator; }

	private:
		ECommandListType            m_Type;
		Com<ID3D12CommandAllocator> m_Allocator;
	};
} // namespace Brainfryer::DX12