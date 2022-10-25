#pragma once

#include "Brainfryer/Renderer/Format.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr DXGI_FORMAT DX12Format(EFormat format)
	{
		switch (format)
		{
		case EFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
		case EFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EFormat::R32_UINT: return DXGI_FORMAT_R32_UINT;
		case EFormat::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
		case EFormat::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EFormat::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
} // namespace Brainfryer::DX12