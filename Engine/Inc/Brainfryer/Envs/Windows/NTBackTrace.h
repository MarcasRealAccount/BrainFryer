#pragma once

#include "Brainfryer/Utils/BackTrace.h"
#include "Win32.h"

namespace Brainfryer::Windows
{
	Utils::BackTrace CaptureBackTrace(std::uint32_t skip, std::uint32_t maxFrames);
}