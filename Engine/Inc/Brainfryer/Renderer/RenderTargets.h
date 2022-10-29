#pragma once

#include "ImageView.h"

#include <memory>
#include <vector>

namespace Brainfryer
{
	struct RenderTargetsInfo
	{
	public:
		std::vector<FrameImageView> colorViews;
		FrameImageView              depthStencil = { nullptr };
	};

	class RenderTargets
	{
	public:
		static std::unique_ptr<RenderTargets> Create(const RenderTargetsInfo& info);

	public:
		virtual ~RenderTargets() = default;

		virtual void update(std::uint32_t index) = 0;

		virtual std::uint32_t colorCount() const      = 0;
		virtual bool          hasDepthStencil() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer