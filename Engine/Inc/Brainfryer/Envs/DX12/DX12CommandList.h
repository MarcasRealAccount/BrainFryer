#pragma once

#include "Brainfryer/Renderer/CommandList.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	class DX12CommandAllocator;

	constexpr D3D12_COMMAND_LIST_TYPE DX12CommandListType(ECommandListType type)
	{
		switch (type)
		{
		case ECommandListType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case ECommandListType::Bundle: return D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case ECommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case ECommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
		case ECommandListType::VideoDecode: return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
		case ECommandListType::VideoProcess: return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
		case ECommandListType::VideoEncode: return D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;
		}
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	class DX12CommandList : public CommandList
	{
	public:
		DX12CommandList(DX12CommandAllocator* allocator);
		DX12CommandList(DX12CommandAllocator* allocator, ID3D12Device10* device);
		DX12CommandList(DX12CommandList&& move) noexcept;
		~DX12CommandList();

		virtual void begin() override;
		virtual void end() override;

		virtual ECommandListType type() const override;

		auto& handle() { return m_CommandList; }
		auto& handle() const { return m_CommandList; }

	private:
		DX12CommandAllocator*           m_Allocator;
		Com<ID3D12GraphicsCommandList7> m_CommandList;
	};
} // namespace Brainfryer::DX12