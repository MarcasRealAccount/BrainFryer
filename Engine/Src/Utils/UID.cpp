#include "Brainfryer/Utils/UID.h"

#include <random>

namespace Brainfryer
{
	static std::mt19937_64 s_UIDRng { std::random_device {}() };

	UID UID::Random(std::uint16_t type)
	{
		return { type, s_UIDRng() };
	}
} // namespace Brainfryer