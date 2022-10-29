#pragma once

namespace Brainfryer
{
	enum class EFormat
	{
		Unknown,
		R8G8B8A8_UNORM,
		R16_UINT,
		R16G16_FLOAT,
		R16G16B16A16_FLOAT,
		R32_UINT,
		R32G32_FLOAT,
		R32G32B32_FLOAT,
		R32G32B32A32_FLOAT,
		D24_UNORM_S8_UINT,
		D32_FLOAT,
		D32_FLOAT_S8_UINT_24_UB,
	};

	constexpr bool FormatIsDepthStencil(EFormat format)
	{
		switch (format)
		{
		case EFormat::D24_UNORM_S8_UINT:
		case EFormat::D32_FLOAT:
		case EFormat::D32_FLOAT_S8_UINT_24_UB:
			return true;
		default:
			return false;
		}
	}
} // namespace Brainfryer