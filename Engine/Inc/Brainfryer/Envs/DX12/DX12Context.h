#pragma once

#include "Brainfryer/Renderer/Context.h"
#include "DX12.h"

#include <unordered_map>
#include <vector>

namespace Brainfryer::DX12
{
	class DX12CommandAllocator;
	class DX12CommandList;

	class DX12Context : public Context
	{
	public:
		static constexpr EContextAPI API = EContextAPI::DX12;

	private:
		static void DebugMessageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* pContext);

	public:
		DX12Context();
		~DX12Context();

		virtual void executeCommandLists(const std::vector<CommandList*>& commandLists) override;

		virtual void          waitForGPU() override;
		virtual UID           newCMDList() override;
		virtual void          destroyCMDList(UID id) override;
		virtual CommandList*  currentCMDList(UID id = {}) override;
		virtual CommandList*  nextFrame() override;
		virtual std::uint32_t frameIndex() const override;
		virtual std::uint32_t frameCount() const override;

		virtual bool initialized() const override { return m_Device.valid(); }

		virtual EContextAPI api() const override { return API; }

		auto& factory() { return m_Factory; }
		auto& factory() const { return m_Factory; }
		auto& device() { return m_Device; }
		auto& device() const { return m_Device; }
		auto& commandQueue() { return m_CommandQueue; }
		auto& commandQueue() const { return m_CommandQueue; }

	private:
		Com<IDXGIFactory7>      m_Factory;
		Com<ID3D12Device9>      m_Device;
		Com<ID3D12InfoQueue1>   m_InfoQueue;
		std::uint32_t           m_DMCCookie;
		Com<ID3D12CommandQueue> m_CommandQueue;

		std::vector<DX12CommandAllocator> m_CommandAllocators;

		std::unordered_map<UID, std::unique_ptr<DX12CommandList[]>> m_CommandListAllocationMap;

		std::uint32_t              m_FrameIndex;
		Com<ID3D12Fence1>          m_FrameFence;
		HANDLE                     m_FrameEvent;
		std::vector<std::uint64_t> m_FrameValues;

		std::uint32_t m_FrameCount;
	};
} // namespace Brainfryer::DX12