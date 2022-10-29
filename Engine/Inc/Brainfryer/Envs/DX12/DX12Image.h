#pragma once

#include "Brainfryer/Renderer/Image.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_RESOURCE_FLAGS DX12ImageFlags(EImageFlags flags)
	{
		std::uint32_t out = 0;
		if (flags & ImageFlags::AllowRenderTarget) out |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (flags & ImageFlags::AllowDepthStencil) out |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		return static_cast<D3D12_RESOURCE_FLAGS>(out);
	}

	constexpr D3D12_RESOURCE_STATES DX12ImageState(EImageState state)
	{
		std::uint32_t dx12State = D3D12_RESOURCE_STATE_COMMON;
		if (state & ImageState::RenderTarget) dx12State |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (state & ImageState::DepthWrite) dx12State |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		if (state & ImageState::DepthRead) dx12State |= D3D12_RESOURCE_STATE_DEPTH_READ;
		if (state & ImageState::UnorderedAccess) dx12State |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if (state & ImageState::NonPixelShaderResource) dx12State |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (state & ImageState::PixelShaderResource) dx12State |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (state & ImageState::IndirectArgument) dx12State |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (state & ImageState::CopyDst) dx12State |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (state & ImageState::CopySrc) dx12State |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if (state & ImageState::ResolveDst) dx12State |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		if (state & ImageState::ResolveSrc) dx12State |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		if (state & ImageState::ShadingRateSrc) dx12State |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		return static_cast<D3D12_RESOURCE_STATES>(dx12State);
	}

	constexpr D3D12_RESOURCE_DIMENSION DX12ImageType(EImageType type)
	{
		switch (type)
		{
		case EImageType::_1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case EImageType::_1DArray: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case EImageType::_2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case EImageType::_2DArray: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case EImageType::_3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		return D3D12_RESOURCE_DIMENSION_BUFFER;
	}

	class DX12Image : public Image
	{
	public:
		DX12Image(const ImageInfo& info);
		~DX12Image();

		virtual void resize(std::uint16_t width, std::uint16_t height = 1, std::uint16_t depth = 1) override;

		virtual void copyFrom(CommandList* commandList, BufferImageView bufferView, Point3D destOffset = { 0, 0, 0 }, Rect3D bufferRect = { -1, -1, -1, 0, 0, 0 }) override;

		virtual void transition(CommandList* commandList, EImageState state) override;

		virtual std::uint64_t requiredBufferSize() override;

		virtual EImageType    type() const override { return m_Type; }
		virtual EFormat       format() const override { return m_Format; }
		virtual EImageState   state() const override { return m_State; }
		virtual std::uint16_t width() const override { return static_cast<std::uint16_t>(m_Size.w); }
		virtual std::uint16_t height() const override { return static_cast<std::uint16_t>(m_Size.h); }
		virtual std::uint16_t depth() const override { return static_cast<std::uint16_t>(m_Size.d); }
		virtual Size3D        size() const override { return m_Size; }

		virtual bool isRenderTarget() const override { return m_Flags & ImageFlags::AllowRenderTarget; };
		virtual bool isDepthStencil() const override { return m_Flags & ImageFlags::AllowDepthStencil; };

		virtual bool initialized() const { return m_Resource.valid(); }

		auto& handle() { return m_Resource; }
		auto& handle() const { return m_Resource; }

		auto& clearValue() const { return m_ClearValue; }

	private:
		Com<ID3D12Resource2> m_Resource;
		EImageType           m_Type;
		EFormat              m_Format;
		EImageState          m_State;
		EImageFlags          m_Flags;
		std::uint32_t        m_Alignment;
		Size3D               m_Size;
		ClearValue           m_ClearValue;
	};

	class DX12FrameImage : public FrameImage
	{
	public:
		DX12FrameImage(const FrameImageInfo& info);
		~DX12FrameImage();

		virtual void resize(std::uint32_t index, std::uint16_t width, std::uint16_t height = 1, std::uint16_t depth = 1) override;

		virtual void copyFrom(CommandList* commandList, std::uint32_t index, BufferImageView bufferView, Point3D destOffset = { 0, 0, 0 }, Rect3D bufferRect = { -1, -1, -1, 0, 0, 0 }) override;

		virtual void transition(CommandList* commandList, std::uint32_t index, EImageState state) override;

		virtual std::uint64_t requiredBufferSize() override;

		virtual EImageType    type() const override { return m_Type; }
		virtual EFormat       format() const override { return m_Format; }
		virtual EImageState   state(std::uint32_t index) const override { return index < m_States.size() ? m_States[index] : ImageState::Common; }
		virtual std::uint16_t width(std::uint32_t index) const override { return index < m_Resources.size() ? static_cast<std::uint16_t>(m_Sizes[index].w) : 0; }
		virtual std::uint16_t height(std::uint32_t index) const override { return index < m_Resources.size() ? static_cast<std::uint16_t>(m_Sizes[index].h) : 0; }
		virtual std::uint16_t depth(std::uint32_t index) const override { return index < m_Resources.size() ? static_cast<std::uint16_t>(m_Sizes[index].d) : 0; }
		virtual Size3D        size(std::uint32_t index) const override { return index < m_Resources.size() ? m_Sizes[index] : Size3D { 0, 0, 0 }; }
		virtual std::uint32_t imageCount() const override { return static_cast<std::uint32_t>(m_Resources.size()); }

		virtual bool isRenderTarget() const override { return m_Flags & ImageFlags::AllowRenderTarget; };
		virtual bool isDepthStencil() const override { return m_Flags & ImageFlags::AllowDepthStencil; };

		virtual bool initialized() const { return !m_Resources.empty() && m_Resources[0].valid(); }

		auto& resources() { return m_Resources; }
		auto& resources() const { return m_Resources; }

		auto& clearValue() const { return m_ClearValue; }

	private:
		std::vector<Com<ID3D12Resource2>> m_Resources;
		std::vector<EImageState>          m_States;
		std::vector<Size3D>               m_Sizes;

		EImageType    m_Type;
		EFormat       m_Format;
		EImageFlags   m_Flags;
		std::uint32_t m_Alignment;
		ClearValue    m_ClearValue;
	};
} // namespace Brainfryer::DX12