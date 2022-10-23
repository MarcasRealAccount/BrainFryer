#pragma once

#include "Brainfryer/Renderer/ImageAddressMode.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_TEXTURE_ADDRESS_MODE DX12ImageAddressMode(EImageAddressMode mode)
	{
		switch (mode)
		{
		case EImageAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case EImageAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case EImageAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case EImageAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		}
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
} // namespace Brainfryer::DX12