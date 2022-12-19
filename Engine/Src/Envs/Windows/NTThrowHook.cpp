#include "Brainfryer/Envs/Windows/NTThrowHook.h"
#include "Brainfryer/Envs/Windows/NTBackTrace.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"
#include "Brainfryer/Utils/Exception.h"

#include <mutex>

namespace Brainfryer::Windows
{
	static void*      g_EET; // g_ExcludedExceptionType
	static std::mutex g_EETMutex;

	static thread_local void* g_VectoredExceptionHandler;

	static LONG ExcludeExceptionHandler(EXCEPTION_POINTERS* ExceptionPointers);
	static LONG VectoredHandler(EXCEPTION_POINTERS* ExceptionPointers);

	void NTHookThrow()
	{
		g_EETMutex.lock();
		if (!g_EET)
		{
			void* handle = AddVectoredExceptionHandler(~0U, &ExcludeExceptionHandler);

			try
			{
				throw Utils::Exception("", "", CaptureBackTrace(0, 0));
			}
			catch ([[maybe_unused]] const Utils::Exception& e)
			{
			}

			RemoveVectoredExceptionHandler(handle);
		}
		g_EETMutex.unlock();

		AddVectoredExceptionHandler(~0U, &VectoredHandler);
	}

	LONG ExcludeExceptionHandler(EXCEPTION_POINTERS* ExceptionPointers)
	{
		g_EET = reinterpret_cast<void*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[2]);
		return 0;
	}

	LONG VectoredHandler(EXCEPTION_POINTERS* ExceptionPointers)
	{
		if (ExceptionPointers->ExceptionRecord->ExceptionInformation[0] == 429065504ULL)
		{
			if (reinterpret_cast<void*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[2]) != g_EET)
				Utils::LastBackTrace() = CaptureBackTrace(6, 32);
			else
				Utils::LastBackTrace() = {};
		}
		else
		{
			Utils::LastBackTrace() = CaptureBackTrace(1, 32);
		}
		return 0;
	}
} // namespace Brainfryer::Windows