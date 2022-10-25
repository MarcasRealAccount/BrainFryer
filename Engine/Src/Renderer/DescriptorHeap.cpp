#include "Brainfryer/Renderer/DescriptorHeap.h"
#include "Brainfryer/Envs/DX12/DX12DescriptorHeap.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<DescriptorHeap> DescriptorHeap::Create(const DescriptorHeapInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12DescriptorHeap>(info);
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer