#include "Brainfryer/Renderer/Image.h"
#include "Brainfryer/Envs/DX12/DX12Image.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<Image> Image::Create(const ImageInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12Image>(info);
			else
				return nullptr;
		}
		return nullptr;
	}

	std::unique_ptr<FrameImage> FrameImage::Create(const FrameImageInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12FrameImage>(info);
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer