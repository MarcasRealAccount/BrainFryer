#pragma once

#include "Brainfryer/Window/Window.h"
#include "Format.h"

namespace Brainfryer
{
	class CommandList;

	struct SwapchainInfo
	{
	public:
		Window*       window;
		std::uint32_t bufferCount;
	};

	class Swapchain
	{
	public:
		static std::unique_ptr<Swapchain> Create(const SwapchainInfo& info);

	public:
		virtual ~Swapchain() = default;

		virtual void bind(CommandList* commandList)                                      = 0;
		virtual void clear(CommandList* commandList, float r, float g, float b, float a) = 0;
		virtual void unbind(CommandList* commandList)                                    = 0;
		virtual void present()                                                           = 0;

		virtual std::uint32_t imageIndex() const = 0;
		virtual EFormat       format() const     = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer