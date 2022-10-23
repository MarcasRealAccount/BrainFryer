#pragma once

#include "Brainfryer/Renderer/Buffer.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_HEAP_TYPE DX12HeapType(EHeapType type)
	{
		switch (type)
		{
		case EHeapType::Default: return D3D12_HEAP_TYPE_DEFAULT;
		case EHeapType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
		}
		return D3D12_HEAP_TYPE_DEFAULT;
	}

	constexpr D3D12_RESOURCE_STATES DX12BufferState(EBufferState state)
	{
		std::uint32_t stateIn = static_cast<std::uint32_t>(state);

		std::uint32_t dx12State = D3D12_RESOURCE_STATE_COMMON;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::VertexAndConstant)) dx12State |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::Index)) dx12State |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::UnorderedAccess)) dx12State |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::StreamOut)) dx12State |= D3D12_RESOURCE_STATE_STREAM_OUT;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::IndirectArgument)) dx12State |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::CopyDst)) dx12State |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (stateIn & static_cast<std::uint32_t>(EBufferState::CopySrc)) dx12State |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		return static_cast<D3D12_RESOURCE_STATES>(dx12State);
	}

	class DX12Buffer : public Buffer
	{
	public:
		DX12Buffer(const BufferInfo& info);
		~DX12Buffer();

		virtual void* map() override;
		virtual void  unmap() override;

		virtual void transition(CommandList* commandList, EBufferState state) override;

		virtual EHeapType    heapType() const override { return m_HeapType; }
		virtual EBufferState state() const override { return m_State; }

		virtual bool initialized() const override { return m_Resource.valid(); }

		auto& handle() { return m_Resource; }
		auto& handle() const { return m_Resource; }

	private:
		Com<ID3D12Resource2> m_Resource;
		EHeapType            m_HeapType;
		EBufferState         m_State;
	};
} // namespace Brainfryer::DX12