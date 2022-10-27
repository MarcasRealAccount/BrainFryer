#pragma once

#include <cstdint>

namespace Brainfryer
{
	struct Point
	{
	public:
		std::int32_t x, y;
	};

	struct Point3D
	{
	public:
		std::int32_t x, y, z;
	};

	struct Size
	{
	public:
		std::uint32_t w, h;
	};

	struct Size3D
	{
	public:
		std::uint32_t w, h, d;
	};
} // namespace Brainfryer