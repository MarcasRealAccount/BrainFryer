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

		virtual void copyFrom(CommandList* commandList, BufferImageView bufferView, Point3D destOffset = { 0, 0, 0 }, Rect3D bufferRect = { -1, -1, -1, 0, 0, 0 }) override;

		virtual void transition(CommandList* commandList, EImageState state) override;

		virtual std::uint64_t requiredBufferSize() override;

		virtual EImageType    type() const override { return m_Type; }
		virtual EFormat       format() const override { return m_Format; }
		virtual EImageState   state() const override { return m_State; }
		virtual std::uint16_t width() const override { return m_Width; }
		virtual std::uint16_t height() const override { return m_Height; }
		virtual std::uint16_t depth() const override { return m_Depth; }

		virtual bool isRenderTarget() const override { return m_Flags & ImageFlags::AllowRenderTarget; };
		virtual bool isDepthStencil() const override { return m_Flags & ImageFlags::AllowDepthStencil; };

		virtual bool initialized() const { return m_Resource.valid(); }

		auto& handle() { return m_Resource; }
		auto& handle() const { return m_Resource; }

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