#pragma once

#include "Brainfryer/Renderer/DescriptorHeap.h"
#include "Brainfryer/Renderer/ImageType.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_DESCRIPTOR_HEAP_TYPE DX12DescriptorHeapType(EDescriptorHeapType type)
	{
		switch (type)
		{
		case EDescriptorHeapType::SRV_CBV_UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case EDescriptorHeapType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		}
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}

	constexpr D3D12_SRV_DIMENSION DX12ImageViewType(EImageType type)
	{
		switch (type)
		{
		case EImageType::_1D: return D3D12_SRV_DIMENSION_TEXTURE1D;
		case EImageType::_1DArray: return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
		case EImageType::_2D: return D3D12_SRV_DIMENSION_TEXTURE2D;
		case EImageType::_2DArray: return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		case EImageType::_3D: return D3D12_SRV_DIMENSION_TEXTURE3D;
		}
		return D3D12_SRV_DIMENSION_TEXTURE2D;
	}

	class DX12DescriptorHeap : public DescriptorHeap
	{
	public:
		DX12DescriptorHeap(const DescriptorHeapInfo& info);
		~DX12DescriptorHeap();

		virtual bool valid(std::uint32_t index) const override;
		virtual void incRef(std::uint32_t index) override;
		virtual void decRef(std::uint32_t index) override;

		virtual DescriptorHeapRef              createBufferView(BufferView view, std::uint32_t stride = 0, bool isRaw = false) override;
		virtual DescriptorHeapRef              createImageView(ImageView view) override;
		virtual std::vector<DescriptorHeapRef> createFrameImageViews(FrameImageView view) override;

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle(std::uint32_t index) const;

		virtual EDescriptorHeapType type() const override { return m_Type; }
		virtual std::uint32_t       capacity() const override { return m_Capacity; }
		virtual std::uint32_t       size() const override { return m_Size; }
		virtual bool                shaderVisible() const override { return m_ShaderVisible; }

		virtual bool initialized() const override { return m_Heap.valid(); }

		auto& handle() { return m_Heap; }
		auto& handle() const { return m_Heap; }

	private:
		std::uint32_t findFree() const;

	private:
		EDescriptorHeapType         m_Type;
		std::uint32_t               m_Capacity;
		std::uint32_t               m_Size;
		bool                        m_ShaderVisible;
		std::uint32_t               m_SearchStart;
		std::vector<std::uint64_t>  m_AllocationMap;
		std::vector<std::uint32_t>  m_RefCounts;
		Com<ID3D12DescriptorHeap>   m_Heap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GPUStart;
		std::uint32_t               m_HeapInc;
	};
} // namespace Brainfryer::DX12