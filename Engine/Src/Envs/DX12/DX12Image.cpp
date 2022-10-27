#include "Brainfryer/Envs/DX12/DX12Image.h"
#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"

#include <fusion.h>

namespace Brainfryer::DX12
{
	DX12Image::DX12Image(const ImageInfo& info)
	    : m_Type(info.type),
	      m_Format(info.format),
	      m_Flags(info.flags),
	      m_State(info.initialState),
	      m_Width(info.width),
	      m_Height(info.height),
	      m_Depth(info.depth)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_HEAP_PROPERTIES heapProperties {};
		heapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask     = 0;
		heapProperties.VisibleNodeMask      = 0;

		D3D12_RESOURCE_DESC1 desc {};
		desc.Dimension = DX12ImageType(m_Type);
		desc.Alignment = info.alignment;
		switch (info.type)
		{
		case EImageType::_1D:
			desc.Width            = m_Width;
			desc.Height           = 1;
			desc.DepthOrArraySize = 1;
			break;
		case EImageType::_1DArray:
			desc.Width            = m_Width;
			desc.Height           = 1;
			desc.DepthOrArraySize = m_Depth;
			break;
		case EImageType::_2D:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = 1;
			break;
		case EImageType::_2DArray:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = m_Depth;
			break;
		case EImageType::_3D:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = m_Depth;
			break;
		}
		desc.DepthOrArraySize   = 1;
		desc.MipLevels          = 1;
		desc.Format             = DX12Format(m_Format);
		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout             = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
		desc.Flags              = DX12ImageFlags(m_Flags);

		desc.SamplerFeedbackMipRegion.Width  = 0;
		desc.SamplerFeedbackMipRegion.Height = 0;
		desc.SamplerFeedbackMipRegion.Depth  = 0;

		D3D12_CLEAR_VALUE clearValue {};
		// TODO(MarcasRealAccount): Check if format is depth stencil
		bool hasClearValue = m_ClearValue.color[0] >= 0.0f;
		if (hasClearValue)
		{
			clearValue.Format = desc.Format;
			std::memcpy(clearValue.Color, m_ClearValue.color, sizeof(clearValue.Color));
			//clearValue.DepthStencil.Depth   = m_ClearValue.ds.depth;
			//clearValue.DepthStencil.Stencil = static_cast<std::uint8_t>(m_ClearValue.ds.stencil);
		}

		HRVLog(context->device()->CreateCommittedResource2(
		    &heapProperties,
		    D3D12_HEAP_FLAG_NONE,
		    &desc,
		    DX12ImageState(m_State & ~(ImageState::CopySrc | ImageState::CopyDst)),
		    hasClearValue ? &clearValue : nullptr,
		    nullptr,
		    m_Resource, m_Resource));
	}

	DX12Image::~DX12Image()
	{
	}

	void DX12Image::copyFrom(CommandList* commandList, BufferImageView bufferView, Point3D destOffset, Rect3D bufferRect)
	{
		D3D12_TEXTURE_COPY_LOCATION dst {};
		dst.pResource        = m_Resource.get();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION src {};
		src.pResource = static_cast<DX12Buffer*>(bufferView.buffer)->handle().get();
		src.Type      = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		src.PlacedFootprint.Footprint.Format   = DX12Format(bufferView.format);
		src.PlacedFootprint.Footprint.Width    = bufferView.width;
		src.PlacedFootprint.Footprint.Height   = bufferView.height;
		src.PlacedFootprint.Footprint.Depth    = bufferView.depth;
		src.PlacedFootprint.Footprint.RowPitch = bufferView.rowPitch;

		src.PlacedFootprint.Offset = bufferView.offset;

		bool      hasSrcBox = bufferRect.x >= 0 && bufferRect.y >= 0 && bufferRect.z >= 0;
		D3D12_BOX srcBox {};
		if (hasSrcBox)
		{
			srcBox.left   = bufferRect.x;
			srcBox.top    = bufferRect.y;
			srcBox.front  = bufferRect.z;
			srcBox.right  = bufferRect.x + bufferRect.w;
			srcBox.bottom = bufferRect.y + bufferRect.h;
			srcBox.back   = bufferRect.z + bufferRect.d;
		}

		static_cast<DX12CommandList*>(commandList)->handle()->CopyTextureRegion(&dst, destOffset.x, destOffset.y, destOffset.z, &src, hasSrcBox ? &srcBox : nullptr);
	}

	void DX12Image::transition(CommandList* commandList, EImageState state)
	{
		if (m_State == state)
			return;

		D3D12_RESOURCE_BARRIER barrier {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_Resource.get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = DX12ImageState(m_State);
		barrier.Transition.StateAfter  = DX12ImageState(state);
		static_cast<DX12CommandList*>(commandList)->handle()->ResourceBarrier(1, &barrier);
		m_State = state;
	}

	std::uint64_t DX12Image::requiredBufferSize()
	{
		auto context = Context::Get<DX12Context>();
		auto desc    = m_Resource->GetDesc1();

		std::uint64_t size;
		context->device()->GetCopyableFootprints1(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &size);
		return size;
	}

	DX12FrameImage::DX12FrameImage(const FrameImageInfo& info)
	    : m_Resources(Context::FrameCount()),
	      m_States(Context::FrameCount(), info.initialState),
	      m_Type(info.type),
	      m_Format(info.format),
	      m_Flags(info.flags),
	      m_Width(info.width),
	      m_Height(info.height),
	      m_Depth(info.depth)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_HEAP_PROPERTIES heapProperties {};
		heapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask     = 0;
		heapProperties.VisibleNodeMask      = 0;

		D3D12_RESOURCE_DESC1 desc {};
		desc.Dimension = DX12ImageType(m_Type);
		desc.Alignment = info.alignment;
		switch (info.type)
		{
		case EImageType::_1D:
			desc.Width            = m_Width;
			desc.Height           = 1;
			desc.DepthOrArraySize = 1;
			break;
		case EImageType::_1DArray:
			desc.Width            = m_Width;
			desc.Height           = 1;
			desc.DepthOrArraySize = m_Depth;
			break;
		case EImageType::_2D:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = 1;
			break;
		case EImageType::_2DArray:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = m_Depth;
			break;
		case EImageType::_3D:
			desc.Width            = m_Width;
			desc.Height           = m_Height;
			desc.DepthOrArraySize = m_Depth;
			break;
		}
		desc.DepthOrArraySize   = 1;
		desc.MipLevels          = 1;
		desc.Format             = DX12Format(m_Format);
		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout             = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
		desc.Flags              = DX12ImageFlags(m_Flags);

		desc.SamplerFeedbackMipRegion.Width  = 0;
		desc.SamplerFeedbackMipRegion.Height = 0;
		desc.SamplerFeedbackMipRegion.Depth  = 0;

		D3D12_CLEAR_VALUE clearValue {};
		// TODO(MarcasRealAccount): Check if format is depth stencil
		bool hasClearValue = m_ClearValue.color[0] >= 0.0f;
		if (hasClearValue)
		{
			clearValue.Format = desc.Format;
			std::memcpy(clearValue.Color, m_ClearValue.color, sizeof(clearValue.Color));
			//clearValue.DepthStencil.Depth   = m_ClearValue.ds.depth;
			//clearValue.DepthStencil.Stencil = static_cast<std::uint8_t>(m_ClearValue.ds.stencil);
		}

		for (std::uint32_t i = 0; i < m_Resources.size(); ++i)
			HRVLog(context->device()->CreateCommittedResource2(
			    &heapProperties,
			    D3D12_HEAP_FLAG_NONE,
			    &desc,
			    DX12ImageState(m_States[i] & ~(ImageState::CopySrc | ImageState::CopyDst)),
			    hasClearValue ? &clearValue : nullptr,
			    nullptr,
			    m_Resources[i], m_Resources[i]));
	}

	DX12FrameImage::~DX12FrameImage()
	{
	}

	void DX12FrameImage::copyFrom(CommandList* commandList, std::uint32_t index, BufferImageView bufferView, Point3D destOffset, Rect3D bufferRect)
	{
		if (index >= m_Resources.size())
			return;

		D3D12_TEXTURE_COPY_LOCATION dst {};
		dst.pResource        = m_Resources[index].get();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION src {};
		src.pResource = static_cast<DX12Buffer*>(bufferView.buffer)->handle().get();
		src.Type      = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		src.PlacedFootprint.Footprint.Format   = DX12Format(bufferView.format);
		src.PlacedFootprint.Footprint.Width    = bufferView.width;
		src.PlacedFootprint.Footprint.Height   = bufferView.height;
		src.PlacedFootprint.Footprint.Depth    = bufferView.depth;
		src.PlacedFootprint.Footprint.RowPitch = bufferView.rowPitch;

		src.PlacedFootprint.Offset = bufferView.offset;

		bool      hasSrcBox = bufferRect.x >= 0 && bufferRect.y >= 0 && bufferRect.z >= 0;
		D3D12_BOX srcBox {};
		if (hasSrcBox)
		{
			srcBox.left   = bufferRect.x;
			srcBox.top    = bufferRect.y;
			srcBox.front  = bufferRect.z;
			srcBox.right  = bufferRect.x + bufferRect.w;
			srcBox.bottom = bufferRect.y + bufferRect.h;
			srcBox.back   = bufferRect.z + bufferRect.d;
		}

		static_cast<DX12CommandList*>(commandList)->handle()->CopyTextureRegion(&dst, destOffset.x, destOffset.y, destOffset.z, &src, hasSrcBox ? &srcBox : nullptr);
	}

	void DX12FrameImage::transition(CommandList* commandList, std::uint32_t index, EImageState state)
	{
		if (index >= m_Resources.size() || m_States[index] == state)
			return;

		D3D12_RESOURCE_BARRIER barrier {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_Resources[index].get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = DX12ImageState(m_States[index]);
		barrier.Transition.StateAfter  = DX12ImageState(state);
		static_cast<DX12CommandList*>(commandList)->handle()->ResourceBarrier(1, &barrier);
		m_States[index] = state;
	}

	std::uint64_t DX12FrameImage::requiredBufferSize()
	{
		auto context = Context::Get<DX12Context>();
		auto desc    = m_Resources[0]->GetDesc1();

		std::uint64_t size;
		context->device()->GetCopyableFootprints1(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &size);
		return size;
	}
} // namespace Brainfryer::DX12