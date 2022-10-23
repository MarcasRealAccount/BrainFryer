#pragma once

#include "Brainfryer/Renderer/Image.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_RESOURCE_FLAGS DX12ImageFlags(EImageFlags flags)
	{
		std::uint32_t flg = static_cast<std::uint32_t>(flags);
		std::uint32_t out = 0;
		if (flg & static_cast<std::uint32_t>(EImageFlags::AllowRenderTarget)) out |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (flg & static_cast<std::uint32_t>(EImageFlags::AllowDepthStencil)) out |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		return static_cast<D3D12_RESOURCE_FLAGS>(out);
	}

	constexpr D3D12_RESOURCE_STATES DX12ImageState(EImageState state)
	{
		std::uint32_t stateIn = static_cast<std::uint32_t>(state);

		std::uint32_t dx12State = D3D12_RESOURCE_STATE_COMMON;
		if (stateIn & static_cast<std::uint32_t>(EImageState::RenderTarget)) dx12State |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (stateIn & static_cast<std::uint32_t>(EImageState::DepthWrite)) dx12State |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		if (stateIn & static_cast<std::uint32_t>(EImageState::DepthRead)) dx12State |= D3D12_RESOURCE_STATE_DEPTH_READ;
		if (stateIn & static_cast<std::uint32_t>(EImageState::UnorderedAccess)) dx12State |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (stateIn & static_cast<std::uint32_t>(EImageState::NonPixelShaderResource)) dx12State |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (stateIn & static_cast<std::uint32_t>(EImageState::PixelShaderResource)) dx12State |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (stateIn & static_cast<std::uint32_t>(EImageState::IndirectArgument)) dx12State |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (stateIn & static_cast<std::uint32_t>(EImageState::CopyDst)) dx12State |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (stateIn & static_cast<std::uint32_t>(EImageState::CopySrc)) dx12State |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if (stateIn & static_cast<std::uint32_t>(EImageState::ResolveDst)) dx12State |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		if (stateIn & static_cast<std::uint32_t>(EImageState::ResolveSrc)) dx12State |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		if (stateIn & static_cast<std::uint32_t>(EImageState::ShadingRateSrc)) dx12State |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		return static_cast<D3D12_RESOURCE_STATES>(dx12State);
	}

	class DX12Image : public Image
	{
	public:
		DX12Image(const ImageInfo& info);
		~DX12Image();

		virtual void copyFrom(CommandList* commandList, BufferImageView bufferView, Point3D destOffset = { -1, -1, -1 }, Rect3D bufferRect = { -1, -1, -1, 0, 0, 0 }) override;

		virtual void transition(CommandList* commandList, EImageState state) override;

		virtual std::uint64_t requiredBufferSize() override;

		virtual EImageType    type() const override { return m_Type; }
		virtual EFormat       format() const override { return m_Format; }
		virtual std::uint16_t width() const override { return m_Width; }
		virtual std::uint16_t height() const override { return m_Height; }
		virtual std::uint16_t depth() const override { return m_Depth; }

		virtual bool isRenderTarget() const override { return m_Flags & EImageFlags::AllowRenderTarget; };
		virtual bool isDepthStencil() const override { return m_Flags & EImageFlags::AllowDepthStencil; };

		virtual bool initialized() const { return m_Resource.valid(); }

	private:
		Com<ID3D12Resource2> m_Resource;
		EImageType           m_Type;
		EFormat              m_Format;
		EImageState          m_State;
		EImageFlags          m_Flags;
		std::uint16_t        m_Width;
		std::uint16_t        m_Height;
		std::uint16_t        m_Depth;
	};
} // namespace Brainfryer::DX12