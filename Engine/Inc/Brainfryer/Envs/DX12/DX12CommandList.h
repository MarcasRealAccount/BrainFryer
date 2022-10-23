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
		DX12CommandList(DX12CommandAllocator* allocator, ID3D12Device9* device);
		DX12CommandList(DX12CommandList&& move) noexcept;
		~DX12CommandList();

		virtual void begin(GraphicsPipeline* initialPipeline = nullptr) override;
		virtual void end() override;

		virtual void setPrimitiveTopology(EPrimitiveTopology topology) override;
		virtual void setVertexBuffers(std::uint32_t startIndex, const std::vector<VertexBufferView>& buffers) override;
		virtual void setIndexBuffer(IndexBufferView indexBuffer) override;

		virtual void drawInstanced(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t startVertex, std::uint32_t startInstance) override;
		virtual void drawIndexedInstanced(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t startIndex, std::uint32_t startVertex, std::uint32_t startInstance) override;

		virtual ECommandListType type() const override;

		auto& handle() { return m_CommandList; }
		auto& handle() const { return m_CommandList; }

	private:
		DX12CommandAllocator*           m_Allocator;
		Com<ID3D12GraphicsCommandList6> m_CommandList;
	};
} // namespace Brainfryer::DX12