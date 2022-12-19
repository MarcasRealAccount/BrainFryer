#include "Brainfryer/Utils/Exception.h"
#include "Brainfryer/Envs/Windows/NTThrowHook.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::Utils
{
	static thread_local BackTrace tl_BackTrace;

	void HookThrow()
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::NTHookThrow();
	}

	BackTrace& LastBackTrace() { return tl_BackTrace; }
} // namespace Brainfryer::Utils
