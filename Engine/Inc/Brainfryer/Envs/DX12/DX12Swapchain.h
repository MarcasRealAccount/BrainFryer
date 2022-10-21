#pragma once

#include "Brainfryer/Renderer/Swapchain.h"
#include "DX12.h"

#include <vector>

namespace Brainfryer::DX12
{
	class DX12Swapchain : public Swapchain
	{
	public:
		DX12Swapchain(const SwapchainInfo& info);
		~DX12Swapchain();

		virtual void bind(CommandList* commandList) override;
		virtual void clear(CommandList* commandList, float r, float g, float b, float a) override;
		virtual void unbind(CommandList* commandList) override;
		virtual void present() override;

		virtual std::uint32_t imageIndex() const override;

		virtual bool initialized() const override { return m_Swapchain.valid(); }

		auto handle() { return m_Swapchain.get(); }
		auto handle() const { return m_Swapchain.get(); }

	private:
		Window* m_Window;

		Com<IDXGISwapChain4>             m_Swapchain;
		std::vector<Com<ID3D12Resource>> m_Images;
		Com<ID3D12DescriptorHeap>        m_DescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE      m_HeapStart;
		std::size_t                      m_HeapIncrement;

		std::uint32_t m_ImageIndex;
	};
} // namespace Brainfryer::DX12