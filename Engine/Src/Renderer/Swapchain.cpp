#include "Brainfryer/Renderer/Swapchain.h"
#include "Brainfryer/Envs/DX12/DX12Swapchain.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<Swapchain> Swapchain::Create(const SwapchainInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12Swapchain>(info);
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer