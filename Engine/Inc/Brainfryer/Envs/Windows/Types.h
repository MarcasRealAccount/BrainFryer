#pragma once

#include "Brainfryer/Utils/Core.h"

namespace Brainfryer::Windows::Types
{
	using HANDLE    = void*;
	using HLOCAL    = HANDLE;
	using HINSTANCE = struct HISTANCE__*;
	using HMODULE   = HINSTANCE;
	using HWND      = struct HWND__*;
	using HDC       = struct HDC__*;
	using HMONITOR  = struct HMONITOR__*;
	using HMENU     = struct HMENU__*;
	using HICON     = struct HICON__*;
	using HBRUSH    = struct HBRUSH__*;
	using HCURSOR   = struct HCURSOR__*;

	using BYTE      = std::uint8_t;
	using SHORT     = std::int16_t;
	using USHORT    = std::uint16_t;
	using INT       = std::int32_t;
	using UINT      = std::uint32_t;
	using LONG      = std::int32_t;
	using ULONG     = std::uint32_t;
	using LONGLONG  = std::int64_t;
	using ULONGLONG = std::uint64_t;
	using WORD      = std::uint16_t;
	using DWORD     = std::uint32_t;
	using DWORD64   = std::uint64_t;
	using QWORD     = std::uint64_t;
	using FLOAT     = float;
	using DOUBLE    = double;
	using BOOL      = std::int32_t;
	using ATOM      = std::uint32_t;
	using INT_PTR   = std::int64_t;
	using UINT_PTR  = std::uint64_t;
	using LONG_PTR  = std::int64_t;
	using ULONG_PTR = std::uint64_t;
	using SIZE_T    = ULONG_PTR;
	using LRESULT   = LONG_PTR;
	using WPARAM    = UINT_PTR;
	using LPARAM    = LONG_PTR;

	using LPVOID  = void*;
	using LPCVOID = const void*;
	using LPSTR   = char*;
	using LPCSTR  = const char*;
	using LPWSTR  = wchar_t*;
	using LPCWSTR = const wchar_t*;

	using LARGE_INTEGER = union _LARGE_INTEGER
	{
		struct
		{
			DWORD LowPart;
			LONG  HighPart;
		} DUMMYSTRUCTNAME;
		struct
		{
			DWORD LowPart;
			LONG  HighPart;
		} u;
		LONGLONG QuadPart;
	};

	using LUID = struct _LUID
	{
		DWORD LowPart;
		LONG  HighPart;
	};

	using POINT = struct tagPOINT
	{
		LONG x;
		LONG y;
	};

	using RECT = struct tagRECT
	{
		LONG left;
		LONG top;
		LONG right;
		LONG bottom;
	};

	using SECURITY_ATTRIBUTES = struct _SECURITY_ATTRIBUTES
	{
		DWORD  nLength;
		LPVOID lpSecurityDescriptor;
		bool   bInheritHandle;
	};
} // namespace Brainfryer::Windows::Types