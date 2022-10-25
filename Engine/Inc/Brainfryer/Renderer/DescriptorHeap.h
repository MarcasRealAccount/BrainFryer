#pragma once

#include "BufferView.h"
#include "DescriptorHeapRef.h"
#include "ImageView.h"

#include <memory>
#include <vector>

namespace Brainfryer
{
	enum class EDescriptorHeapType
	{
		SRV_CBV_UAV,
		Sampler
	};

	struct DescriptorHeapInfo
	{
	public:
		EDescriptorHeapType type;
		std::uint32_t       capacity;
		bool                shaderVisible;
	};

	class DescriptorHeap
	{
	public:
		static std::unique_ptr<DescriptorHeap> Create(const DescriptorHeapInfo& info);

	public:
		virtual ~DescriptorHeap() = default;

		virtual bool valid(std::uint32_t index) const = 0;
		virtual void incRef(std::uint32_t index)      = 0;
		virtual void decRef(std::uint32_t index)      = 0;

		virtual DescriptorHeapRef              createBufferView(BufferView view, std::uint32_t stride = 0, bool isRaw = false) = 0;
		virtual DescriptorHeapRef              createImageView(ImageView view)                                                 = 0;
		virtual std::vector<DescriptorHeapRef> createFrameImageViews(FrameImageView view)                                      = 0;

		virtual EDescriptorHeapType type() const          = 0;
		virtual std::uint32_t       capacity() const      = 0;
		virtual std::uint32_t       size() const          = 0;
		virtual bool                shaderVisible() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer