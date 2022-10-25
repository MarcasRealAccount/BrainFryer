#include "Brainfryer/Renderer/RenderTargets.h"
#include "Brainfryer/Envs/DX12/DX12RenderTargets.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<RenderTargets> RenderTargets::Create(const RenderTargetsInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12RenderTargets>(info);
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer