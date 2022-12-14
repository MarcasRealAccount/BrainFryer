#pragma once

#include "ImageType.h"

#include <cstdint>

namespace Brainfryer
{
	class Image;
	class FrameImage;

	struct ImageView
	{
	public:
		Image*        image;
		EImageType    type;
		std::uint32_t mostDetailedMip;
		std::uint32_t mipLevels;
		std::uint32_t firstArraySlice;
		std::uint32_t arraySize;
		std::uint32_t planeSlice;
		float         minLODClamp;
	};

	struct FrameImageView
	{
	public:
		FrameImage*   image;
		EImageType    type;
		std::uint32_t mostDetailedMip;
		std::uint32_t mipLevels;
		std::uint32_t firstArraySlice;
		std::uint32_t arraySize;
		std::uint32_t planeSlice;
		float         minLODClamp;
	};
} // namespace Brainfryer