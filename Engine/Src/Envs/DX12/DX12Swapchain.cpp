#include "Brainfryer/Envs/DX12/DX12Swapchain.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"

#include <format>
#include <iostream>

namespace Brainfryer::DX12
{
	DX12Swapchain::DX12Swapchain(const SwapchainInfo& info)
	    : m_Window(info.window),
	      m_WindowRect({ 0, 0, 0, 0 }),
	      m_HeapStart({ 0 }),
	      m_HeapIncrement(0),
	      m_ImageIndex(0)
	{
		auto  context      = Context::Get<DX12Context>();
		auto& factory      = context->factory();
		auto& commandQueue = context->commandQueue();

		m_WindowRect = m_Window->windowRect();

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
		swapchainDesc.Width  = m_WindowRect.w;
		swapchainDesc.Height = m_WindowRect.h;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = false;

		swapchainDesc.SampleDesc.Count   = 1;
		swapchainDesc.SampleDesc.Quality = 0;

		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = info.bufferCount;
		swapchainDesc.Scaling     = DXGI_SCALING_NONE;
		swapchainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDesc.Flags       = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		Com<IDXGISwapChain1> swapchain;
		HRVLT(factory->CreateSwapChainForHwnd(
		    commandQueue.get(),
		    static_cast<Windows::Win32Window*>(m_Window)->nativeHandle(),
		    &swapchainDesc,
		    nullptr,
		    nullptr,
		    swapchain));
		m_Swapchain  = swapchain;
		m_ImageIndex = m_Swapchain->GetCurrentBackBufferIndex();

		m_Swapchain->GetDesc1(&swapchainDesc);
		m_Images.resize(swapchainDesc.BufferCount);

		auto& device = context->device();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc {};
		rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = swapchainDesc.BufferCount;
		rtvHeapDesc.NodeMask       = 0;
		rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRVLT(device->CreateDescriptorHeap(&rtvHeapDesc, m_DescriptorHeap, m_DescriptorHeap));

		m_HeapStart     = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_HeapIncrement = device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		auto handle     = m_HeapStart;
		for (std::uint32_t i = 0; i < swapchainDesc.BufferCount; ++i)
		{
			auto& renderTarget = m_Images[i];
			HRVLT(m_Swapchain->GetBuffer(i, renderTarget, renderTarget));
			device->CreateRenderTargetView(renderTarget.get(), nullptr, handle);
			handle.ptr += m_HeapIncrement;
		}
	}

	DX12Swapchain::~DX12Swapchain()
	{
	}

	void DX12Swapchain::bind(CommandList* commandList)
	{
		auto& commandListHandle = static_cast<DX12CommandList*>(commandList)->handle();

		auto rect = m_Window->windowRect();
		if (rect.w != m_WindowRect.w || rect.h != m_WindowRect.h)
		{
			auto context = Context::Get<DX12Context>();
			m_Images.clear();
			m_DescriptorHeap.release();

			context->waitForGPU();

			HRVLT(m_Swapchain->ResizeBuffers(0, rect.w, rect.h, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));

			m_ImageIndex = m_Swapchain->GetCurrentBackBufferIndex();

			DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
			m_Swapchain->GetDesc1(&swapchainDesc);
			m_Images.resize(swapchainDesc.BufferCount);

			auto& device = context->device();

			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc {};
			rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.NumDescriptors = swapchainDesc.BufferCount;
			rtvHeapDesc.NodeMask       = 0;
			rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HRVLT(device->CreateDescriptorHeap(&rtvHeapDesc, m_DescriptorHeap, m_DescriptorHeap));

			m_HeapStart     = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_HeapIncrement = device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
			auto handle     = m_HeapStart;
			for (std::uint32_t i = 0; i < swapchainDesc.BufferCount; ++i)
			{
				auto& renderTarget = m_Images[i];
				HRVLT(m_Swapchain->GetBuffer(i, renderTarget, renderTarget));
				device->CreateRenderTargetView(renderTarget.get(), nullptr, handle);
				handle.ptr += m_HeapIncrement;
			}

			m_WindowRect = rect;
		}

		D3D12_VIEWPORT viewport {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width    = static_cast<float>(rect.w);
		viewport.Height   = static_cast<float>(rect.h);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		D3D12_RECT scissor {};
		scissor.left   = 0;
		scissor.top    = 0;
		scissor.right  = rect.w;
		scissor.bottom = rect.h;
		commandListHandle->RSSetViewports(1, &viewport);
		commandListHandle->RSSetScissorRects(1, &scissor);

		D3D12_RESOURCE_BARRIER barrier {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_Images[m_ImageIndex].get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = 0;
		commandListHandle->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_HeapStart;
		handle.ptr += m_ImageIndex * m_HeapIncrement;
		commandListHandle->OMSetRenderTargets(1, &handle, false, nullptr);
	}

	void DX12Swapchain::clear(CommandList* commandList, float r, float g, float b, float a)
	{
		auto& commandListHandle = static_cast<DX12CommandList*>(commandList)->handle();

		FLOAT clearColor[4] { r, g, b, a };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_HeapStart;
		handle.ptr += m_ImageIndex * m_HeapIncrement;
		commandListHandle->ClearRenderTargetView(handle, clearColor, 0, nullptr);
	}

	void DX12Swapchain::unbind(CommandList* commandList)
	{
		auto& commandListHandle = static_cast<DX12CommandList*>(commandList)->handle();

		D3D12_RESOURCE_BARRIER barrier {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_Images[m_ImageIndex].get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = 0;
		commandListHandle->ResourceBarrier(1, &barrier);
	}

	void DX12Swapchain::present()
	{
		HRVLT(m_Swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
		m_ImageIndex = m_Swapchain->GetCurrentBackBufferIndex();
	}

	std::uint32_t DX12Swapchain::imageIndex() const
	{
		return m_ImageIndex;
	}
} // namespace Brainfryer::DX12