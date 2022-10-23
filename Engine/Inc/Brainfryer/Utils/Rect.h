#pragma once

#include <cstdint>

namespace Brainfryer
{
	struct Rect
	{
	public:
		std::int32_t  x, y;
		std::uint32_t w, h;
	};

	struct Rect3D
	{
	public:
		std::int32_t  x, y, z;
		std::uint32_t w, h, d;
	};
} // namespace Brainfryer