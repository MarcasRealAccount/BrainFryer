#include "Brainfryer/Utils/Intrinsics.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::Utils::Intrinsics
{
	extern "C"
	{
		unsigned char _BitScanForward(unsigned long* index, unsigned long mask);
		unsigned char _BitScanForward64(unsigned long* index, unsigned long long mask);

		int __builtin_ffs(int x);
		int __builtin_ffsl(long x);
		int __builtin_ffsll(long long x);
	}

	std::uint32_t BitScanForward(std::uint32_t value)
	{
		if (!value)
			return ~0U;

		if constexpr (Core::s_IsToolsetMSVC)
		{
			unsigned long index;
			_BitScanForward(&index, static_cast<unsigned long>(value));
			return static_cast<std::uint32_t>(index);
		}
		else if constexpr (Core::s_IsToolsetClang || Core::s_IsToolsetGCC)
		{
			return static_cast<std::uint32_t>(__builtin_ffs(static_cast<int>(value)) - 1);
		}
		else
		{
			for (std::uint8_t i = 0; i < 32; ++i)
				if (value & (1U << i))
					return i;
			return ~0U;
		}
	}

	std::uint32_t BitScanForward64(std::uint64_t value)
	{
		if (!value)
			return ~0U;

		if constexpr (Core::s_IsToolsetMSVC)
		{
			unsigned long index;
			_BitScanForward64(&index, static_cast<unsigned long long>(value));
			return static_cast<std::uint32_t>(index);
		}
		else if constexpr (Core::s_IsToolsetClang || Core::s_IsToolsetGCC)
		{
			return static_cast<std::uint32_t>(__builtin_ffsll(static_cast<long long>(value)) - 1);
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