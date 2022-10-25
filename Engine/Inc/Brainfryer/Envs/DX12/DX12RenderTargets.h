#pragma once

#include "Brainfryer/Renderer/ImageType.h"
#include "Brainfryer/Renderer/RenderTargets.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	class DX12Image;

	constexpr D3D12_RTV_DIMENSION DX12RTVViewDimension(EImageType type)
	{
		switch (type)
		{
		case EImageType::_1D: return D3D12_RTV_DIMENSION_TEXTURE1D;
		case EImageType::_1DArray: return D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
		case EImageType::_2D: return D3D12_RTV_DIMENSION_TEXTURE2D;
		case EImageType::_2DArray: return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		case EImageType::_3D: return D3D12_RTV_DIMENSION_TEXTURE3D;
		}
		return D3D12_RTV_DIMENSION_TEXTURE2D;
	}

	constexpr D3D12_DSV_DIMENSION DX12DSVViewDimension(EImageType type)
	{
		switch (type)
		{
		case EImageType::_1D: return D3D12_DSV_DIMENSION_TEXTURE1D;
		case EImageType::_1DArray: return D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
		case EImageType::_2D: return D3D12_DSV_DIMENSION_TEXTURE2D;
		case EImageType::_2DArray: return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		}
		return D3D12_DSV_DIMENSION_TEXTURE2D;
	}

	class DX12RenderTargets : public RenderTargets
	{
	public:
		DX12RenderTargets(const RenderTargetsInfo& info);
		~DX12RenderTargets();

		virtual std::uint32_t colorCount() const override { return static_cast<std::uint32_t>(m_Colors.size()); }
		virtual bool          hasDepthStencil() const override { return !!m_DepthStencil.image; }

		virtual bool initialized() const override { return m_RTVHeap.valid(); }

		D3D12_CPU_DESCRIPTOR_HANDLE getRTVStart(std::uint32_t frameIndex) const;
		D3D12_CPU_DESCRIPTOR_HANDLE getDSVStart(std::uint32_t frameIndex) const;
		D3D12_CPU_DESCRIPTOR_HANDLE getRTV(std::uint32_t index, std::uint32_t frameIndex) const;

		auto& colors() { return m_Colors; }
		auto& colors() const { return m_Colors; }
		auto& depthStencil() { return m_DepthStencil; }
		auto& depthStencil() const { return m_DepthStencil; }

	private:
		Com<ID3D12DescriptorHeap>   m_RTVHeap;
		Com<ID3D12DescriptorHeap>   m_DSVHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVStart;
		D3D12_CPU_DESCRIPTOR_HANDLE m_DSVStart;
		std::uint32_t               m_RTVHeapInc;
		std::uint32_t               m_DSVHeapInc;
		std::vector<FrameImageView> m_Colors;
		FrameImageView              m_DepthStencil;
	};
} // namespace Brainfryer::DX12