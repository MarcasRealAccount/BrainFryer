#include "Brainfryer/Renderer/PipelineLayout.h"
#include "Brainfryer/Envs/DX12/DX12PipelineLayout.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	PipelineLayoutParameter::PipelineLayoutParameter(EPipelineLayoutParameterType type, PipelineLayoutDescriptor descriptor, EShaderVisibility visibility)
	    : type(type),
	      visibility(visibility),
	      value(descriptor) {}

	PipelineLayoutParameter::PipelineLayoutParameter(PipelineLayoutConstants constants, EShaderVisibility visibility)
	    : type(EPipelineLayoutParameterType::Constants),
	      visibility(visibility),
	      value(constants) {}

	PipelineLayoutParameter::PipelineLayoutParameter(PipelineLayoutDescriptorTable descriptorTable, EShaderVisibility visibility)
	    : type(EPipelineLayoutParameterType::DescriptorTable),
	      visibility(visibility),
	      value(std::move(descriptorTable)) {}

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