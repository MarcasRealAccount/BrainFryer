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
		case EFormat::R16_UINT: return DXGI_FORMAT_R16_UINT;
		case EFormat::R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
		case EFormat::R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case EFormat::R32_UINT: return DXGI_FORMAT_R32_UINT;
		case EFormat::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
		case EFormat::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EFormat::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case EFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case EFormat::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
		case EFormat::D32_FLOAT_S8_UINT_24_UB: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
} // namespace Brainfryer::DX12