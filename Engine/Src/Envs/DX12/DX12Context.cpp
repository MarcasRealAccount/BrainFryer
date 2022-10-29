#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12CommandAllocator.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Utils/Log.h"

namespace Brainfryer::DX12
{
	void DX12Context::DebugMessageCallback([[maybe_unused]] D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, [[maybe_unused]] D3D12_MESSAGE_ID id, LPCSTR description, [[maybe_unused]] void* pContext)
	{
		auto logger = Log::GetOrCreateLogger("DX12");
		switch (severity)
		{
		case D3D12_MESSAGE_SEVERITY_MESSAGE:
			logger->info("{}", description);
			break;
		case D3D12_MESSAGE_SEVERITY_INFO:
			logger->debug("{}", description);
			break;
		case D3D12_MESSAGE_SEVERITY_WARNING:
			logger->warn("{}", description);
			break;
		case D3D12_MESSAGE_SEVERITY_ERROR:
			logger->error("{}", description);
			break;
		case D3D12_MESSAGE_SEVERITY_CORRUPTION:
			logger->critical("{}", description);
			break;
		}
	}

	DX12Context::DX12Context()
	    : m_DMCCookie(0),
	      m_FrameIndex(0),
	      m_FrameEvent(nullptr),
	      m_FrameCount(2)
	{
		UINT dxgiFactoryFlags = 0;

		if constexpr (Core::s_IsConfigDebug)
		{
			Com<ID3D12Debug> debugController;
			if (HRValidate(D3D12GetDebugInterface(debugController, debugController)))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}

		HRVLT(CreateDXGIFactory2(dxgiFactoryFlags, m_Factory, m_Factory));

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

		HRVLT(D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_2, m_Device, m_Device));

		if constexpr (Core::s_IsConfigDebug)
			if (HRValidate(m_Device->QueryInterface(m_InfoQueue, m_InfoQueue)))
				m_InfoQueue->RegisterMessageCallback(&DebugMessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &m_DMCCookie);

		D3D12_COMMAND_QUEUE_DESC queueDesc {};
		queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;

		HRVLT(m_Device->CreateCommandQueue(&queueDesc, m_CommandQueue, m_CommandQueue));

		m_FrameValues.resize(m_FrameCount, 0);

		auto allocators = std::make_unique<DX12CommandAllocator[]>(m_FrameCount);
		auto cmdLists   = std::make_unique<DX12CommandList[]>(m_FrameCount);
		for (std::uint32_t i = 0; i < m_FrameCount; ++i)
		{
			new (&allocators[i]) DX12CommandAllocator(m_Device.get(), ECommandListType::Direct);
			new (&cmdLists[i]) DX12CommandList(&allocators[i], m_Device.get());
		}
		m_CommandAllocatorsMap.insert_or_assign({}, std::move(allocators));
		m_CommandListAllocationMap.insert_or_assign({}, std::move(cmdLists));

		HRVLT(m_Device->CreateFence(m_FrameValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, m_FrameFence, m_FrameFence));

		m_FrameEvent = Windows::CreateEventW(nullptr, false, false, nullptr); // TODO(MarcasRealAccount): Add Windows Event name?
		if (!m_FrameEvent)
			return;
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
		HRVLT(m_CommandQueue->Signal(m_FrameFence.get(), m_FrameValues[m_FrameIndex]));
		HRVLT(m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent));
		Windows::WaitForSingleObjectEx(m_FrameEvent, Windows::INFINITE, false);

		++m_FrameValues[m_FrameIndex];
	}

	UID DX12Context::newCMDList()
	{
		UID id = UID::Random(0);
		while (m_CommandListAllocationMap.find(id) != m_CommandListAllocationMap.end())
			id = UID::Random(0);

		auto allocators = std::make_unique<DX12CommandAllocator[]>(m_FrameCount);
		auto cmdLists   = std::make_unique<DX12CommandList[]>(m_FrameCount);
		for (std::uint32_t i = 0; i < m_FrameCount; ++i)
		{
			new (&allocators[i]) DX12CommandAllocator(m_Device.get(), ECommandListType::Direct);
			new (&cmdLists[i]) DX12CommandList(&allocators[i], m_Device.get());
		}
		m_CommandAllocatorsMap.insert_or_assign(id, std::move(allocators));
		m_CommandListAllocationMap.insert_or_assign(id, std::move(cmdLists));
		return id;
	}

	void DX12Context::destroyCMDList(UID id)
	{
		auto itr = m_CommandListAllocationMap.find(id);
		if (itr == m_CommandListAllocationMap.end())
			return;
		m_CommandListAllocationMap.erase(itr);
	}

	CommandList* DX12Context::currentCMDList(UID id)
	{
		auto itr = m_CommandListAllocationMap.find(id);
		if (itr == m_CommandListAllocationMap.end())
			return nullptr;
		return &itr->second[m_FrameIndex];
	}

	CommandList* DX12Context::nextFrame(UID id)
	{
		if (id == 0)
		{
			m_FrameIndex = (m_FrameIndex + 1) % m_FrameCount;

			std::uint64_t currentValue = m_FrameValues[m_FrameIndex];
			HRVLT(m_CommandQueue->Signal(m_FrameFence.get(), currentValue));

			if (m_FrameFence->GetCompletedValue() < m_FrameValues[m_FrameIndex])
			{
				HRVLT(m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent));
				Windows::WaitForSingleObjectEx(m_FrameEvent, Windows::INFINITE, false);
			}

			m_FrameValues[m_FrameIndex] = currentValue + 1;
		}

		m_CommandAllocatorsMap[id][m_FrameIndex].reset();
		return &m_CommandListAllocationMap[id][m_FrameIndex];
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