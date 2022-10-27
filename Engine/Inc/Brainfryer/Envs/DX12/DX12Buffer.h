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
		std::uint32_t dx12State = D3D12_RESOURCE_STATE_COMMON;
		if (state & BufferState::VertexAndConstant) dx12State |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		if (state & BufferState::Index) dx12State |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		if (state & BufferState::UnorderedAccess) dx12State |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (state & BufferState::NonPixelShaderResource) dx12State |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (state & BufferState::IndirectArgument) dx12State |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (state & BufferState::CopyDst) dx12State |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (state & BufferState::CopySrc) dx12State |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		return static_cast<D3D12_RESOURCE_STATES>(dx12State);
	}

	class DX12Buffer : public Buffer
	{
	public:
		DX12Buffer(const BufferInfo& info);
		~DX12Buffer();

		virtual void* map(std::uint64_t readStart = 0, std::uint64_t readSize = 0) override;
		virtual void  unmap(std::uint64_t writeStart = 0, std::uint64_t writeSize = 0, bool explicitWriteRange = false) override;

		virtual void copyFrom(CommandList* commandList, BufferView view, std::uint64_t offset = 0) override;

		virtual void transition(CommandList* commandList, EBufferState state) override;

		virtual EHeapType     heapType() const override { return m_HeapType; }
		virtual EBufferState  state() const override { return m_State; }
		virtual std::uint64_t size() const override { return m_Size; }

		virtual bool initialized() const override { return m_Resource.valid(); }

		auto& handle() { return m_Resource; }
		auto& handle() const { return m_Resource; }

	private:
		Com<ID3D12Resource2> m_Resource;
		EHeapType            m_HeapType;
		EBufferState         m_State;
		std::uint64_t        m_Size;
	};

	class DX12FrameBuffer : public FrameBuffer
	{
	public:
		DX12FrameBuffer(const FrameBufferInfo& info);
		~DX12FrameBuffer();

		virtual void* map(std::uint32_t index, std::uint64_t readStart = 0, std::uint64_t readSize = 0) override;
		virtual void  unmap(std::uint32_t index, std::uint64_t writeStart = 0, std::uint64_t writeSize = 0, bool explicitWriteRange = false) override;

		virtual void copyFrom(CommandList* commandList, std::uint32_t index, BufferView view, std::uint64_t offset = 0) override;

		virtual void transition(CommandList* commandList, std::uint32_t index, EBufferState state) override;

		virtual EHeapType     heapType() const override { return m_HeapType; }
		virtual EBufferState  state(std::uint32_t index) const override { return index < m_States.size() ? m_States[index] : BufferState::Common; }
		virtual std::uint64_t size() const override { return m_Size; }
		virtual std::uint32_t bufferCount() const override { return static_cast<std::uint32_t>(m_Resources.size()); }

		virtual bool initialized() const override { return !m_Resources.empty() && m_Resources[0].valid(); }

		auto& resources() { return m_Resources; }
		auto& resources() const { return m_Resources; }

	private:
		std::vector<Com<ID3D12Resource2>> m_Resources;
		std::vector<EBufferState>         m_States;

		EHeapType     m_HeapType;
		std::uint64_t m_Size;
	};
} // namespace Brainfryer::DX12