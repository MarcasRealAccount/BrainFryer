#pragma once

#include <cstdint>

namespace Brainfryer::Utils::Intrinsics
{
	std::uint32_t BitScanForward(std::uint32_t value);
	std::uint32_t BitScanForward64(std::uint64_t value);
} // namespace Brainfryer::Utils::Intrinsics