#include "Brainfryer/Utils/Exception.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"
#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Utils/Log.h"

namespace Brainfryer::Utils
{
#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
	static void* g_ExcludedExceptionType;
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
	using ExceptionFuncPtr = void (*)(void*, void*, void (*)(void*));
	static ExceptionFuncPtr      g_OriginalThrow;
	static void*                 g_ExcludedExceptionType;
#endif

	static thread_local BackTrace tl_BackTrace;

#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
	Windows::LONG VectoredHandler(Windows::EXCEPTION_POINTERS* ExceptionPointers);
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
	extern "C" [[noreturn]] void __cxa_throw(void* exception, void* type, void (*dest)(void*));
#endif

	void HookThrow()
	{
#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
		Windows::AddVectoredExceptionHandler(~0U, &VectoredHandler);
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
		void* handle    = Unix::dlopen(nullptr, RTLD_LAZY);
		g_OriginalThrow = reinterpret_cast<decltype(g_OriginalThrow)>(Unix::dlvsym(handle, "__cxa_throw", "CXXABI_1.3"));
		Unix::dlclose(handle);
#endif

		try
		{
			throw Utils::Exception("Hehe", "ehhe");
		}
		catch ([[maybe_unused]] const Utils::Exception& e)
		{
		}
	}

	BackTrace& LastBackTrace() { return tl_BackTrace; }

#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
	Windows::LONG VectoredHandler(Windows::EXCEPTION_POINTERS* ExceptionPointers)
	{
		if (ExceptionPointers->ExceptionRecord->ExceptionInformation[0] == 429065504ULL)
		{
			if (!g_ExcludedExceptionType)
				g_ExcludedExceptionType = reinterpret_cast<void*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[2]);
			if (reinterpret_cast<void*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[2]) != g_ExcludedExceptionType)
				tl_BackTrace = CaptureBackTrace(6, 32);
			else
				tl_BackTrace = {};
		}
		else
			tl_BackTrace = CaptureBackTrace(1, 32);
		return 0;
	}
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
	extern "C"
	{
		[[noreturn]] void __cxa_throw(void* exception, void* type, void (*dest)(void*))
		{
			if (!g_ExcludedExceptionType)
				g_ExcludedExceptionType = type;
			if (type != g_ExcludedExceptionType)
				tl_BackTrace = CaptureBackTrace(1, 32);
			g_OriginalThrow(exception, type, dest);
		}
	}
#endif
} // namespace Brainfryer::Utils
