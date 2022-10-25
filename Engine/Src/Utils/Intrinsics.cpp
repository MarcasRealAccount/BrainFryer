#include "Brainfryer/Utils/Intrinsics.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::Utils::Intrinsics
{
	extern "C"
	{
		unsigned char _BitScanReverse(unsigned long* index, unsigned long mask);
		unsigned char _BitScanReverse64(unsigned long* index, unsigned long long mask);
	}

	std::uint32_t BitScanReverse(std::uint32_t value)
	{
		if constexpr (Core::s_IsToolsetMSVC)
		{
			unsigned long index  = 0;
			auto          result = _BitScanReverse(&index, value);
			return result ? index : ~0U;
		}
		else
		{
			for (std::uint8_t i = 0; i < 32; ++i)
				if (value & (1U << i))
					return i;
			return ~0U;
		}
	}

	std::uint32_t BitScanReverse64(std::uint64_t value)
	{
		if constexpr (Core::s_IsToolsetMSVC)
		{
			unsigned long index  = 0;
			auto          result = _BitScanReverse64(&index, value);
			return result ? index : ~0U;
		}
		else
		{
			for (std::uint8_t i = 0; i < 64; ++i)
				if (value & (1ULL << i))
					return i;
			return ~0U;
		}
	}
} // namespace Brainfryer::Utils::Intrinsics