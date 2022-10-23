#pragma once

#include "Format.h"

#include <cstdint>

namespace Brainfryer
{
	class Buffer;

	struct BufferView
	{
		Buffer*       buffer;
		std::uint64_t offset;
		std::uint32_t size;
	};

	struct VertexBufferView
	{
		Buffer*       buffer;
		std::uint64_t offset;
		std::uint32_t size;
		std::uint32_t stride;
	};

	struct IndexBufferView
	{
		Buffer*       buffer;
		std::uint64_t offset;
		std::uint32_t size;
		EFormat       format;
	};

	struct BufferImageView
	{
		Buffer*       buffer;
		std::uint64_t offset;
		EFormat       format;
		std::uint16_t width;
		std::uint16_t height;
		std::uint16_t depth;
		std::uint32_t rowPitch;
	};
} // namespace Brainfryer