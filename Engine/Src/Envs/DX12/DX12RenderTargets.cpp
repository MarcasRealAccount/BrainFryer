#include "Brainfryer/Envs/DX12/DX12RenderTargets.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"
#include "Brainfryer/Envs/DX12/DX12Image.h"

namespace Brainfryer::DX12
{
	DX12RenderTargets::DX12RenderTargets(const RenderTargetsInfo& info)
	    : m_RTVStart({ 0 }),
	      m_DSVStart({ 0 }),
	      m_RTVHeapInc(0),
	      m_DSVHeapInc(0),
	      m_Colors(info.colorViews),
	      m_DepthStencil(info.depthStencil)
	{
		auto  context = Context::Get<DX12Context>();
		auto& device  = context->device();

		D3D12_DESCRIPTOR_HEAP_DESC desc {};
		desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = context->frameCount() * static_cast<std::uint32_t>(m_Colors.size());
		desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask       = 0;
		if (!HRVLog(device->CreateDescriptorHeap(&desc, m_RTVHeap, m_RTVHeap)))
			return;

		m_RTVStart   = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
		m_RTVHeapInc = device->GetDescriptorHandleIncrementSize(desc.Type);

		for (std::size_t i = 0; i < m_Colors.size(); ++i)
		{
			auto& view = m_Colors[i];

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
			rtvDesc.Format        = DX12Format(view.image->format());
			rtvDesc.ViewDimension = DX12RTVViewDimension(view.type);
			switch (view.type)
			{
			case EImageType::_1D:
				rtvDesc.Texture1D.MipSlice = view.mipLevels;
				break;
			case EImageType::_1DArray:
				rtvDesc.Texture1DArray.MipSlice        = view.mipLevels;
				rtvDesc.Texture1DArray.FirstArraySlice = view.firstArraySlice;
				rtvDesc.Texture1DArray.ArraySize       = view.arraySize;
				break;
			case EImageType::_2D:
				rtvDesc.Texture2D.MipSlice   = view.mipLevels;
				rtvDesc.Texture2D.PlaneSlice = view.planeSlice;
				break;
			case EImageType::_2DArray:
				rtvDesc.Texture2D.MipSlice             = view.mipLevels;
				rtvDesc.Texture2DArray.FirstArraySlice = view.firstArraySlice;
				rtvDesc.Texture2DArray.ArraySize       = view.arraySize;
				rtvDesc.Texture2D.PlaneSlice           = view.planeSlice;
				break;
			case EImageType::_3D:
				rtvDesc.Texture3D.MipSlice    = view.mipLevels;
				rtvDesc.Texture3D.FirstWSlice = view.firstArraySlice;
				rtvDesc.Texture3D.WSize       = view.arraySize;
				break;
			}

			auto& resources = static_cast<DX12FrameImage*>(view.image)->resources();
			for (std::size_t j = 0; j < context->frameCount(); ++j)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE dest = m_RTVStart;
				dest.ptr += (j * m_Colors.size() + i) * m_RTVHeapInc;
				device->CreateRenderTargetView(resources[j].get(), &rtvDesc, dest);
			}
		}

		if (!!m_DepthStencil.image)
		{
			desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.NumDescriptors = context->frameCount();

			if (!HRVLog(device->CreateDescriptorHeap(&desc, m_DSVHeap, m_DSVHeap)))
				return;

			m_DSVStart   = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
			m_DSVHeapInc = device->GetDescriptorHandleIncrementSize(desc.Type);
			auto dest    = m_DSVStart;

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
			dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;
			dsvDesc.Format        = DX12Format(m_DepthStencil.image->format());
			dsvDesc.ViewDimension = DX12DSVViewDimension(m_DepthStencil.type);
			switch (m_DepthStencil.type)
			{
			case EImageType::_1D:
				dsvDesc.Texture1D.MipSlice = m_DepthStencil.mipLevels;
				break;
			case EImageType::_1DArray:
				dsvDesc.Texture1DArray.MipSlice        = m_DepthStencil.mipLevels;
				dsvDesc.Texture1DArray.FirstArraySlice = m_DepthStencil.firstArraySlice;
				dsvDesc.Texture1DArray.ArraySize       = m_DepthStencil.arraySize;
				break;
			case EImageType::_2D:
				dsvDesc.Texture2D.MipSlice = m_DepthStencil.mipLevels;
				break;
			case EImageType::_2DArray:
				dsvDesc.Texture2D.MipSlice             = m_DepthStencil.mipLevels;
				dsvDesc.Texture1DArray.FirstArraySlice = m_DepthStencil.firstArraySlice;
				dsvDesc.Texture1DArray.ArraySize       = m_DepthStencil.arraySize;
				break;
			}
			auto& resources = static_cast<DX12FrameImage*>(m_DepthStencil.image)->resources();
			for (std::uint32_t i = 0; i < context->frameCount(); ++i, dest.ptr += m_DSVHeapInc)
				device->CreateDepthStencilView(resources[i].get(), &dsvDesc, dest);
		}
	}

	DX12RenderTargets::~DX12RenderTargets()
	{
	}

	void DX12RenderTargets::update(std::uint32_t index)
	{
		auto& device = Context::Get<DX12Context>()->device();

		for (std::size_t i = 0; i < m_Colors.size(); ++i)
		{
			auto& view = m_Colors[i];

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
			rtvDesc.Format        = DX12Format(view.image->format());
			rtvDesc.ViewDimension = DX12RTVViewDimension(view.type);
			switch (view.type)
			{
			case EImageType::_1D:
				rtvDesc.Texture1D.MipSlice = view.mipLevels;
				break;
			case EImageType::_1DArray:
				rtvDesc.Texture1DArray.MipSlice        = view.mipLevels;
				rtvDesc.Texture1DArray.FirstArraySlice = view.firstArraySlice;
				rtvDesc.Texture1DArray.ArraySize       = view.arraySize;
				break;
			case EImageType::_2D:
				rtvDesc.Texture2D.MipSlice   = view.mipLevels;
				rtvDesc.Texture2D.PlaneSlice = view.planeSlice;
				break;
			case EImageType::_2DArray:
				rtvDesc.Texture2D.MipSlice             = view.mipLevels;
				rtvDesc.Texture2DArray.FirstArraySlice = view.firstArraySlice;
				rtvDesc.Texture2DArray.ArraySize       = view.arraySize;
				rtvDesc.Texture2D.PlaneSlice           = view.planeSlice;
				break;
			case EImageType::_3D:
				rtvDesc.Texture3D.MipSlice    = view.mipLevels;
				rtvDesc.Texture3D.FirstWSlice = view.firstArraySlice;
				rtvDesc.Texture3D.WSize       = view.arraySize;
				break;
			}

			auto& resources = static_cast<DX12FrameImage*>(view.image)->resources();

			D3D12_CPU_DESCRIPTOR_HANDLE dest = m_RTVStart;
			dest.ptr += (index * m_Colors.size() + i) * m_RTVHeapInc;
			device->CreateRenderTargetView(resources[index].get(), &rtvDesc, dest);
		}

		if (!!m_DepthStencil.image)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
			dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;
			dsvDesc.Format        = DX12Format(m_DepthStencil.image->format());
			dsvDesc.ViewDimension = DX12DSVViewDimension(m_DepthStencil.type);
			switch (m_DepthStencil.type)
			{
			case EImageType::_1D:
				dsvDesc.Texture1D.MipSlice = m_DepthStencil.mipLevels;
				break;
			case EImageType::_1DArray:
				dsvDesc.Texture1DArray.MipSlice        = m_DepthStencil.mipLevels;
				dsvDesc.Texture1DArray.FirstArraySlice = m_DepthStencil.firstArraySlice;
				dsvDesc.Texture1DArray.ArraySize       = m_DepthStencil.arraySize;
				break;
			case EImageType::_2D:
				dsvDesc.Texture2D.MipSlice = m_DepthStencil.mipLevels;
				break;
			case EImageType::_2DArray:
				dsvDesc.Texture2D.MipSlice             = m_DepthStencil.mipLevels;
				dsvDesc.Texture1DArray.FirstArraySlice = m_DepthStencil.firstArraySlice;
				dsvDesc.Texture1DArray.ArraySize       = m_DepthStencil.arraySize;
				break;
			}
			auto& resources = static_cast<DX12FrameImage*>(m_DepthStencil.image)->resources();

			D3D12_CPU_DESCRIPTOR_HANDLE dest = m_DSVStart;
			dest.ptr += index * m_DSVHeapInc;
			device->CreateDepthStencilView(resources[index].get(), &dsvDesc, dest);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTargets::getRTVStart(std::uint32_t frameIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVStart;
		handle.ptr += frameIndex * m_Colors.size() * m_RTVHeapInc;
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTargets::getDSVStart(std::uint32_t frameIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DSVStart;
		handle.ptr += frameIndex * m_DSVHeapInc;
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTargets::getRTV(std::uint32_t index, std::uint32_t frameIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVStart;
		handle.ptr += (frameIndex * m_Colors.size() + index) * m_RTVHeapInc;
		return handle;
	}
} // namespace Brainfryer::DX12