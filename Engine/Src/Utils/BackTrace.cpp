#include "Brainfryer/Utils/BackTrace.h"
#include "Brainfryer/Envs/Windows/NTBackTrace.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::Utils
{
	BackTrace CaptureBackTrace(std::uint32_t skip, std::uint32_t maxFrames)
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::CaptureBackTrace(skip, maxFrames);
		else
			return {};
	}
} // namespace Brainfryer::Utils