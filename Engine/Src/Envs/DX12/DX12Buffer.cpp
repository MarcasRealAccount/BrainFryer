#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"

namespace Brainfryer::DX12
{
	DX12Buffer::DX12Buffer(const BufferInfo& info)
	    : m_HeapType(info.heapType),
	      m_State(info.heapType == EHeapType::Upload ? EBufferState::Common : info.initialState)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_HEAP_PROPERTIES heapProperties {};
		heapProperties.Type                 = DX12HeapType(m_HeapType);
		heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask     = 0;
		heapProperties.VisibleNodeMask      = 0;

		D3D12_RESOURCE_DESC1 desc {};
		desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment          = info.alignment;
		desc.Width              = info.size;
		desc.Height             = 1;
		desc.DepthOrArraySize   = 1;
		desc.MipLevels          = 1;
		desc.Format             = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

		desc.SamplerFeedbackMipRegion.Width  = 0;
		desc.SamplerFeedbackMipRegion.Height = 0;
		desc.SamplerFeedbackMipRegion.Depth  = 0;

		HRVLog(context->device()->CreateCommittedResource2(
		    &heapProperties,
		    D3D12_HEAP_FLAG_NONE,
		    &desc,
		    m_HeapType == EHeapType::Upload ? D3D12_RESOURCE_STATE_GENERIC_READ : DX12BufferState(m_State),
		    nullptr,
		    nullptr,
		    m_Resource, m_Resource));
	}

	DX12Buffer::~DX12Buffer()
	{
	}

	void* DX12Buffer::map(std::uint64_t readStart, std::uint64_t readSize)
	{
		void*       addr = nullptr;
		D3D12_RANGE readRange {};
		readRange.Begin = readStart;
		readRange.End   = readStart + readSize;
		if (HRVLog(m_Resource->Map(0, &readRange, &addr)))
			return addr;
		return nullptr;
	}

	void DX12Buffer::unmap(std::uint64_t writeStart, std::uint64_t writeSize, bool explicitWriteRange)
	{
		D3D12_RANGE writeRange {};
		writeRange.Begin = writeStart;
		writeRange.End   = writeStart + writeSize;
		m_Resource->Unmap(0, explicitWriteRange ? &writeRange : nullptr);
	}

	void DX12Buffer::copyFrom(CommandList* commandList, BufferView view, std::uint64_t offset)
	{
		auto& cmdList = static_cast<DX12CommandList*>(commandList)->handle();
		cmdList->CopyBufferRegion(m_Resource.get(), offset, static_cast<DX12Buffer*>(view.buffer)->m_Resource.get(), view.offset, view.size);
	}

	void DX12Buffer::transition(CommandList* commandList, EBufferState state)
	{
		if (m_HeapType == EHeapType::Upload)
			return;

		auto& cmdList = static_cast<DX12CommandList*>(commandList)->handle();

		// TODO(MarcasRealAccount): Implement enhanced barriers with ID3D12Device10!
		D3D12_RESOURCE_BARRIER barrier {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_Resource.get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = DX12BufferState(m_State);
		barrier.Transition.StateAfter  = DX12BufferState(state);
		cmdList->ResourceBarrier(1, &barrier);
		m_State = state;
	}
} // namespace Brainfryer::DX12