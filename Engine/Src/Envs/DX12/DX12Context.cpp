#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::DX12
{
	DX12Context::DX12Context()
	    : m_FrameIndex(1),
	      m_FrameEvent(nullptr),
	      m_FrameCount(2)
	{
		UINT dxgiFactoryFlags = 0;

#ifdef BUILD_IS_CONFIG_DEBUG
		{
			Com<ID3D12Debug> debugController;
			if (HRValidate(D3D12GetDebugInterface(debugController, debugController)))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		if (!HRValidate(CreateDXGIFactory2(dxgiFactoryFlags, m_Factory, m_Factory)))
			return;

		Com<IDXGIAdapter1> hardwareAdapter;
		for (UINT adapterIndex = 0; HRValidate(m_Factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, hardwareAdapter, hardwareAdapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			hardwareAdapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				hardwareAdapter.release();
				continue;
			}

			if (HRValidate(D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_2, Com<ID3D12Device9>::UUID, nullptr)))
				break;
		}
		if (!hardwareAdapter.valid())
		{
			for (UINT adapterIndex = 0; HRValidate(m_Factory->EnumAdapters1(adapterIndex, hardwareAdapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				hardwareAdapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					hardwareAdapter.release();
					continue;
				}

				if (HRValidate(D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_2, Com<ID3D12Device9>::UUID, nullptr)))
					break;
			}
		}
		if (!hardwareAdapter.valid())
			return;

		if (!HRValidate(D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_2, m_Device, m_Device)))
			return;

		D3D12_COMMAND_QUEUE_DESC queueDesc {};
		queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;

		if (!HRValidate(m_Device->CreateCommandQueue(&queueDesc, m_CommandQueue, m_CommandQueue)))
			return;

		m_CommandAllocators.reserve(m_FrameCount);
		m_CommandLists.reserve(m_FrameCount);
		m_FrameValues.resize(m_FrameCount, 0);

		for (std::uint32_t i = 0; i < m_FrameCount; ++i)
		{
			CommandAllocatorInfo allocatorInfo {};
			allocatorInfo.type = ECommandListType::Direct;
			auto& allocator    = m_CommandAllocators.emplace_back(m_Device.get(), ECommandListType::Direct);

			m_CommandLists.emplace_back(&allocator, m_Device.get());
		}

		if (!HRValidate(m_Device->CreateFence(m_FrameValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, m_FrameFence, m_FrameFence)))
			return;

		m_FrameEvent = Windows::CreateEventW(nullptr, false, false, nullptr); // TODO(MarcasRealAccount): Add Windows Event name?
		if (!m_FrameEvent)
			return;

		waitForGPU();
	}

	DX12Context::~DX12Context()
	{
	}

	void DX12Context::executeCommandLists(const std::vector<CommandList*>& commandLists)
	{
		std::vector<ID3D12CommandList*> lists(commandLists.size());
		for (std::size_t i = 0; i < lists.size(); ++i)
			lists[i] = static_cast<DX12CommandList*>(commandLists[i])->handle().get();
		m_CommandQueue->ExecuteCommandLists(static_cast<std::uint32_t>(lists.size()), lists.data());
	}

	void DX12Context::waitForGPU()
	{
		if (!HRValidate(m_CommandQueue->Signal(m_FrameFence.get(), m_FrameValues[m_FrameIndex])))
			return;

		if (!HRValidate(m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent)))
			return;
		Windows::WaitForSingleObjectEx(m_FrameEvent, Windows::INFINITE, false);

		++m_FrameValues[m_FrameIndex];
	}

	CommandList* DX12Context::nextFrame()
	{
		m_FrameIndex = (m_FrameIndex + 1) % m_FrameCount;

		std::uint64_t currentValue = m_FrameValues[m_FrameIndex];
		if (!HRValidate(m_CommandQueue->Signal(m_FrameFence.get(), currentValue)))
			return nullptr;

		if (m_FrameFence->GetCompletedValue() < m_FrameValues[m_FrameIndex])
		{
			if (!HRValidate(m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent)))
				return nullptr;
			Windows::WaitForSingleObjectEx(m_FrameEvent, Windows::INFINITE, false);
		}

		m_FrameValues[m_FrameIndex] = currentValue + 1;

		m_CommandAllocators[m_FrameIndex].reset();
		return &m_CommandLists[m_FrameIndex];
	}

	std::uint32_t DX12Context::frameIndex() const
	{
		return m_FrameIndex;
	}

	std::uint32_t DX12Context::frameCount() const
	{
		return m_FrameCount;
	}
} // namespace Brainfryer::DX12