#include "Brainfryer/Envs/DX12/DX12DescriptorHeap.h"
#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"
#include "Brainfryer/Envs/DX12/DX12Image.h"
#include "Brainfryer/Utils/Intrinsics.h"

namespace Brainfryer::DX12
{
	DX12DescriptorHeap::DX12DescriptorHeap(const DescriptorHeapInfo& info)
	    : m_Type(info.type),
	      m_Capacity((info.capacity + 63) >> 6 << 6),
	      m_Size(0),
	      m_ShaderVisible(info.shaderVisible),
	      m_SearchStart(0),
	      m_AllocationMap(m_Capacity >> 6),
	      m_RefCounts(m_Capacity),
	      m_CPUStart({ 0 }),
	      m_GPUStart({ 0 }),
	      m_HeapInc(0)
	{
		std::memset(m_AllocationMap.data(), 0xFF, m_AllocationMap.size() * 8);
		auto context = Context::Get<DX12Context>();

		D3D12_DESCRIPTOR_HEAP_DESC desc {};
		desc.Type           = DX12DescriptorHeapType(m_Type);
		desc.NumDescriptors = m_Capacity;
		desc.Flags          = m_ShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask       = 0;

		if (!HRVLog(context->device()->CreateDescriptorHeap(&desc, m_Heap, m_Heap)))
			return;

		m_CPUStart = m_Heap->GetCPUDescriptorHandleForHeapStart();
		m_GPUStart = m_Heap->GetGPUDescriptorHandleForHeapStart();
		m_HeapInc  = context->device()->GetDescriptorHandleIncrementSize(desc.Type);
	}

	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
	}

	bool DX12DescriptorHeap::valid(std::uint32_t index) const
	{
		std::uint32_t i = index >> 6;
		std::uint32_t j = index & 0x3F;
		return !(m_AllocationMap[i] & (1ULL << j));
	}

	void DX12DescriptorHeap::incRef(std::uint32_t index)
	{
		if (!valid(index))
			return;

		++m_RefCounts[index];
	}

	void DX12DescriptorHeap::decRef(std::uint32_t index)
	{
		if (!valid(index))
			return;

		if (--m_RefCounts[index] == 0)
		{
			// TODO(MarcasRealAccount): Request deallocation in framesInFlight number of frames.
			std::uint32_t i = index >> 6;
			std::uint32_t j = index & 0x3F;
			m_AllocationMap[i] |= 1ULL << j;
			m_SearchStart = i;
			--m_Size;
		}
	}

	DescriptorHeapRef DX12DescriptorHeap::createBufferView(BufferView view, std::uint32_t stride, bool isRaw)
	{
		auto context = Context::Get<DX12Context>();

		std::uint32_t freeHeapIndex = findFree();
		m_SearchStart               = freeHeapIndex >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_CPUStart;
		descriptorHandle.ptr += freeHeapIndex * m_HeapInc;

		D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
		desc.Format                     = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
		desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Buffer.FirstElement        = view.offset;
		desc.Buffer.NumElements         = view.size;
		desc.Buffer.StructureByteStride = stride;
		desc.Buffer.Flags               = isRaw ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;

		context->device()->CreateShaderResourceView(static_cast<DX12Buffer*>(view.buffer)->handle().get(), &desc, descriptorHandle);
		m_AllocationMap[m_SearchStart] &= ~(1ULL << (freeHeapIndex & 0x3F));
		++m_Size;
		return DescriptorHeapRef { this, freeHeapIndex };
	}

	DescriptorHeapRef DX12DescriptorHeap::createImageView(ImageView view)
	{
		auto context = Context::Get<DX12Context>();

		std::uint32_t freeHeapIndex = findFree();
		m_SearchStart               = freeHeapIndex >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_CPUStart;
		descriptorHandle.ptr += freeHeapIndex * m_HeapInc;

		D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
		desc.Format                  = DX12Format(view.image->format());
		desc.ViewDimension           = DX12ImageViewType(view.type);
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; //D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		switch (view.type)
		{
		case EImageType::_1D:
			desc.Texture1D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1D.MipLevels           = view.mipLevels;
			desc.Texture1D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_1DArray:
			desc.Texture1DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1DArray.MipLevels           = view.mipLevels;
			desc.Texture1DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture1DArray.ArraySize           = view.arraySize;
			desc.Texture1DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2D:
			desc.Texture2D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2D.MipLevels           = view.mipLevels;
			desc.Texture2D.PlaneSlice          = view.planeSlice;
			desc.Texture2D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2DArray:
			desc.Texture2DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2DArray.MipLevels           = view.mipLevels;
			desc.Texture2DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture2DArray.ArraySize           = view.arraySize;
			desc.Texture2DArray.PlaneSlice          = view.planeSlice;
			desc.Texture2DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_3D:
			desc.Texture3D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture3D.MipLevels           = view.mipLevels;
			desc.Texture3D.ResourceMinLODClamp = view.minLODClamp;
			break;
		}

		context->device()->CreateShaderResourceView(static_cast<DX12Image*>(view.image)->handle().get(), &desc, descriptorHandle);
		m_AllocationMap[m_SearchStart] &= ~(1ULL << (freeHeapIndex & 0x3F));
		++m_Size;
		return DescriptorHeapRef { this, freeHeapIndex };
	}

	std::vector<DescriptorHeapRef> DX12DescriptorHeap::createFrameImageViews(FrameImageView view)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
		desc.Format                  = DX12Format(view.image->format());
		desc.ViewDimension           = DX12ImageViewType(view.type);
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; //D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		switch (view.type)
		{
		case EImageType::_1D:
			desc.Texture1D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1D.MipLevels           = view.mipLevels;
			desc.Texture1D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_1DArray:
			desc.Texture1DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1DArray.MipLevels           = view.mipLevels;
			desc.Texture1DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture1DArray.ArraySize           = view.arraySize;
			desc.Texture1DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2D:
			desc.Texture2D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2D.MipLevels           = view.mipLevels;
			desc.Texture2D.PlaneSlice          = view.planeSlice;
			desc.Texture2D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2DArray:
			desc.Texture2DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2DArray.MipLevels           = view.mipLevels;
			desc.Texture2DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture2DArray.ArraySize           = view.arraySize;
			desc.Texture2DArray.PlaneSlice          = view.planeSlice;
			desc.Texture2DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_3D:
			desc.Texture3D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture3D.MipLevels           = view.mipLevels;
			desc.Texture3D.ResourceMinLODClamp = view.minLODClamp;
			break;
		}

		auto& images = static_cast<DX12FrameImage*>(view.image)->resources();

		std::vector<DescriptorHeapRef> refs(images.size());
		for (std::uint32_t i = 0; i < images.size(); ++i)
		{
			std::uint32_t freeHeapIndex = findFree();
			m_SearchStart               = freeHeapIndex >> 6;

			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_CPUStart;
			descriptorHandle.ptr += freeHeapIndex * m_HeapInc;

			context->device()->CreateShaderResourceView(images[i].get(), &desc, descriptorHandle);
			m_AllocationMap[m_SearchStart] &= ~(1ULL << (freeHeapIndex & 0x3F));
			++m_Size;
			refs[i] = { this, freeHeapIndex };
		}
		return refs;
	}

	DescriptorHeapRef DX12DescriptorHeap::createFrameImageViews(FrameImageView view, std::uint32_t index)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
		desc.Format                  = DX12Format(view.image->format());
		desc.ViewDimension           = DX12ImageViewType(view.type);
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; //D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		switch (view.type)
		{
		case EImageType::_1D:
			desc.Texture1D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1D.MipLevels           = view.mipLevels;
			desc.Texture1D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_1DArray:
			desc.Texture1DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture1DArray.MipLevels           = view.mipLevels;
			desc.Texture1DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture1DArray.ArraySize           = view.arraySize;
			desc.Texture1DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2D:
			desc.Texture2D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2D.MipLevels           = view.mipLevels;
			desc.Texture2D.PlaneSlice          = view.planeSlice;
			desc.Texture2D.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_2DArray:
			desc.Texture2DArray.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture2DArray.MipLevels           = view.mipLevels;
			desc.Texture2DArray.FirstArraySlice     = view.firstArraySlice;
			desc.Texture2DArray.ArraySize           = view.arraySize;
			desc.Texture2DArray.PlaneSlice          = view.planeSlice;
			desc.Texture2DArray.ResourceMinLODClamp = view.minLODClamp;
			break;
		case EImageType::_3D:
			desc.Texture3D.MostDetailedMip     = view.mostDetailedMip;
			desc.Texture3D.MipLevels           = view.mipLevels;
			desc.Texture3D.ResourceMinLODClamp = view.minLODClamp;
			break;
		}

		auto& images = static_cast<DX12FrameImage*>(view.image)->resources();

		std::uint32_t freeHeapIndex = findFree();
		m_SearchStart               = freeHeapIndex >> 6;

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_CPUStart;
		descriptorHandle.ptr += freeHeapIndex * m_HeapInc;

		context->device()->CreateShaderResourceView(images[index].get(), &desc, descriptorHandle);
		m_AllocationMap[m_SearchStart] &= ~(1ULL << (freeHeapIndex & 0x3F));
		++m_Size;
		return { this, freeHeapIndex };
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::descriptorHandle(std::uint32_t index) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_GPUStart;
		handle.ptr += m_HeapInc * index;
		return handle;
	}

	std::uint32_t DX12DescriptorHeap::findFree() const
	{
		if (m_Size == m_Capacity)
			return ~0U;

		for (std::uint32_t i = m_SearchStart; i < m_AllocationMap.size(); ++i)
		{
			std::uint32_t index = Utils::Intrinsics::BitScanForward64(m_AllocationMap[i]);
			if (index != ~0U)
				return i << 6 | index;
		}
		return ~0U;
	}
} // namespace Brainfryer::DX12