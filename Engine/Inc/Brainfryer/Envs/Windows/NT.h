#pragma once

#include "Brainfryer/Utils/Core.h"
#include "Types.h"

#if BUILD_IS_SYSTEM_WINDOWS
#define NTSYSAPI __declspec(dllimport)
#else
#define NTSYSAPI
#endif

namespace Brainfryer::Windows
{
	using namespace Types;

	using SYMBOL_INFOW = struct _SYMBOL_INFOW
	{
		ULONG         SizeOfStruct;
		ULONG         TypeIndex;
		std::uint64_t Reserved[2];
		ULONG         Index;
		ULONG         Size;
		std::uint64_t ModBase;
		ULONG         Flags;
		std::uint64_t Value;
		std::uint64_t Address;
		ULONG         Register;
		ULONG         Scope;
		ULONG         Tag;
		ULONG         NameLen;
		ULONG         MaxNameLen;
		wchar_t       Name[1];
	};

	using IMAGEHLP_LINEW64 = struct _IMAGEHLP_LINEW64
	{
		DWORD         SizeOfStruct;
		void*         Key;
		DWORD         LineNumber;
		LPWSTR        FileName;
		std::uint64_t Address;
	};

	extern "C"
	{
		NTSYSAPI DWORD SymSetOptions(DWORD SymOptions);
		NTSYSAPI bool  SymInitializeW(HANDLE hProcess, LPCWSTR UserSearchPath, bool fInvadeProcess);
		NTSYSAPI bool  SymCleanup(HANDLE hProcess);
		NTSYSAPI bool  SymFromAddrW(HANDLE hProcess, std::uint64_t Address, std::uint64_t* Displacement, SYMBOL_INFOW* Symbol);
		NTSYSAPI bool  SymGetLineFromAddrW64(HANDLE hProcess, std::uint64_t Address, std::uint32_t* Displacement, IMAGEHLP_LINEW64* Line);

		NTSYSAPI WORD RtlCaptureStackBackTrace(DWORD FramesToSkip, DWORD FramesToCapture, void** BackTrace, DWORD* BackTraceHash);
	}

	static constexpr DWORD MAX_SYM_NAME = 2000;

	static constexpr DWORD SYMOPT_CASE_INSENSITIVE          = 0x0000'0001;
	static constexpr DWORD SYMOPT_UNDNAME                   = 0x0000'0002;
	static constexpr DWORD SYMOPT_DEFERRED_LOADS            = 0x0000'0004;
	static constexpr DWORD SYMOPT_NO_CPP                    = 0x0000'0008;
	static constexpr DWORD SYMOPT_LOAD_LINES                = 0x0000'0010;
	static constexpr DWORD SYMOPT_LOAD_ANYTHING             = 0x0000'0040;
	static constexpr DWORD SYMOPT_IGNORE_CVREC              = 0x0000'0080;
	static constexpr DWORD SYMOPT_NO_UNQUALIFIED_LOADS      = 0x0000'0100;
	static constexpr DWORD SYMOPT_FAIL_CRITICAL_ERRORS      = 0x0000'0200;
	static constexpr DWORD SYMOPT_EXACT_SYMBOLS             = 0x0000'0400;
	static constexpr DWORD SYMOPT_ALLOW_ABSOLUTE_SYMBOLS    = 0x0000'0800;
	static constexpr DWORD SYMOPT_IGNORE_NT_SYMPATH         = 0x0000'1000;
	static constexpr DWORD SYMOPT_INCLUDE_32BIT_MODULES     = 0x0000'2000;
	static constexpr DWORD SYMOPT_PUBLICS_ONLY              = 0x0000'4000;
	static constexpr DWORD SYMOPT_NO_PUBLICS                = 0x0000'8000;
	static constexpr DWORD SYMOPT_AUTO_PUBLICS              = 0x0001'0000;
	static constexpr DWORD SYMOPT_NO_IMAGE_SEARCH           = 0x0002'0000;
	static constexpr DWORD SYMOPT_SECURE                    = 0x0004'0000;
	static constexpr DWORD SYMOPT_NO_PROMPTS                = 0x0008'0000;
	static constexpr DWORD SYMOPT_OVERWRITE                 = 0x0010'0000;
	static constexpr DWORD SYMOPT_IGNORE_IMAGEDIR           = 0x0020'0000;
	static constexpr DWORD SYMOPT_FLAT_DIRECTORY            = 0x0040'0000;
	static constexpr DWORD SYMOPT_FAVOR_COMPRESSED          = 0x0080'0000;
	static constexpr DWORD SYMOPT_ALLOW_ZERO_ADDRESS        = 0x0100'0000;
	static constexpr DWORD SYMOPT_DISABLE_SYMSRV_AUTODETECT = 0x0200'0000;
	static constexpr DWORD SYMOPT_DEBUG                     = 0x8000'0000;
} // namespace Brainfryer::Windows