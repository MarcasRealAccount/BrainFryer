#include "Brainfryer/Utils/Exception.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Utils/Log.h"

namespace Brainfryer::Utils
{
#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
	using ExceptionFuncPtr = void (*)(void*, void*);
	static ExceptionFuncPtr g_OriginalThrow;
	static void*            g_ExcludedExceptionType;
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
	using ExceptionFuncPtr = void (*)(void*, void*, void (*)(void*));
	static ExceptionFuncPtr      g_OriginalThrow;
	static void*                 g_ExcludedExceptionType;
#endif

	static thread_local BackTrace tl_BackTrace;

#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
	extern "C" void              _CxxThrowException(void* pExceptionObject, _ThrowInfo* pThrowInfo);
	extern "C" [[noreturn]] void _CxxThrowExceptionHook(void* exception, void* type);
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
	extern "C" [[noreturn]] void __cxa_throw(void* exception, void* type, void (*dest)(void*));
#endif

	void HookThrow()
	{
#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
		std::uint32_t relOffset        = 0;
		auto          originalFunction = &_CxxThrowException;
		relOffset                      = *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(originalFunction) + 1);
		auto subFunction               = reinterpret_cast<decltype(originalFunction)>(reinterpret_cast<std::uint8_t*>(originalFunction) + 5 + relOffset);
		relOffset                      = *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(subFunction) + 2);
		auto subSubFunction            = reinterpret_cast<decltype(originalFunction)>(reinterpret_cast<std::uint8_t*>(subFunction) + 6 + relOffset);

		Windows::DWORD  origProtect = 0;
		Windows::HANDLE hProcess    = Windows::GetCurrentProcess();
		if (!Windows::VirtualProtectEx(hProcess, subSubFunction, 8, Windows::PAGE_READWRITE, &origProtect))
			return;

		g_OriginalThrow = reinterpret_cast<decltype(g_OriginalThrow)>(*reinterpret_cast<std::uintptr_t*>(subSubFunction));

		*reinterpret_cast<std::uintptr_t*>(subSubFunction) = reinterpret_cast<std::uintptr_t>(&_CxxThrowExceptionHook);

		Windows::VirtualProtectEx(hProcess, subSubFunction, 8, origProtect, nullptr);
		Windows::FlushInstructionCache(hProcess, subSubFunction, 8);
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
		void* handle    = Unix::dlopen(nullptr, RTLD_LAZY);
		g_OriginalThrow = reinterpret_cast<decltype(g_OriginalThrow)>(Unix::dlvsym(handle, "__cxa_throw", "CXXABI_1.3"));
		Unix::dlclose(handle);
#endif

		try
		{
			throw Exception("Hehe", "I know right :>");
		}
		catch ([[maybe_unused]] const Exception& e)
		{
		}
	}

	BackTrace& LastBackTrace()
	{
		return tl_BackTrace;
	}

	extern "C"
	{
#if BUILD_IS_TOOLSET_MSVC || (BUILD_IS_TOOLSET_CLANG && BUILD_IS_SYSTEM_WINDOWS)
		[[noreturn]] void _CxxThrowExceptionHook(void* exception, void* type)
		{
			if (!g_ExcludedExceptionType)
				g_ExcludedExceptionType = type;
			if (type != g_ExcludedExceptionType)
				tl_BackTrace = CaptureBackTrace(1, 10);
			g_OriginalThrow(exception, type);
		}
#elif BUILD_IS_TOOLSET_CLANG || BUILD_IS_TOOLSET_GCC
		[[noreturn]] void __cxa_throw(void* exception, void* type, void (*dest)(void*))
		{
			if (!g_ExcludedExceptionType)
				g_ExcludedExceptionType = type;
			if (type != g_ExcludedExceptionType)
				tl_BackTrace = CaptureBackTrace(1, 10);
			g_OriginalThrow(exception, type, dest);
		}
#endif
	}
} // namespace Brainfryer::Utils
