#pragma once

#include <cstdint>

namespace Brainfryer
{
	class Buffer;

	struct VertexBufferView
	{
		Buffer*       buffer;
		std::uint64_t offset;
		std::uint32_t size;
		std::uint32_t stride;
	};
} // namespace Brainfryer