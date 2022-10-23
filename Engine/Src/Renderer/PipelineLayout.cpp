#include "Brainfryer/Renderer/PipelineLayout.h"
#include "Brainfryer/Envs/DX12/DX12PipelineLayout.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<PipelineLayout> PipelineLayout::Create(PipelineLayoutInfo info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12PipelineLayout>(std::move(info));
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer