#pragma once

#include <cstdint>

namespace Brainfryer::Utils::Intrinsics
{
	std::uint32_t BitScanReverse(std::uint32_t value);
	std::uint32_t BitScanReverse64(std::uint64_t value);
} // namespace Brainfryer::Utils::Intrinsics