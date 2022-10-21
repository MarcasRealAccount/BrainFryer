#pragma once

#include "Brainfryer/Utils/Core.h"
#include "Types.h"

#include <bit>
#include <cstdint>

#if BUILD_IS_SYSTEM_WINDOWS
#define WIN32API __declspec(dllimport)
#else
#define WIN32API
#endif

namespace Brainfryer::Windows
{
	using namespace Types;

	using WNDPROC = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);

	using WINDOWPLACEMENT = struct tagWINDOWPLACEMENT
	{
		UINT  length;
		UINT  flags;
		UINT  showCmd;
		POINT ptMinPosition;
		POINT ptMaxPosition;
		RECT  rcNormalPosition;
		RECT  rcDevice;
	};

	using WNDCLASSEXW = struct tagWNDCLASSEXW
	{
		UINT      cbSize;
		UINT      style;
		WNDPROC   lpfnWndProc;
		int       cbClsExtra;
		int       cbWndExtra;
		HINSTANCE hInstance;
		HICON     hIcon;
		HCURSOR   hCursor;
		HBRUSH    hbrBackground;
		LPCWSTR   lpszMenuName;
		LPCWSTR   lpszClassName;
		HICON     hIconSm;
	};

	using MSG = struct tagMSG
	{
		HWND   hwnd;
		UINT   message;
		WPARAM wParam;
		LPARAM lParam;
		DWORD  time;
		POINT  pt;
		DWORD  lPrivate;
	};

	using MONITORINFO = struct tagMONITORINFO
	{
		DWORD cbSize;
		RECT  rcMonitor;
		RECT  rcWork;
		DWORD dwFlags;
	};

	using MONITORINFOEXW = struct tagMONITORINFOEX : public MONITORINFO
	{
		wchar_t szDevice[32];
	};

	extern "C"
	{
		WIN32API int lstrlenW(LPCWSTR lpString);

		WIN32API HLOCAL LocalFree(HLOCAL hMem);

		WIN32API DWORD GetLastError();
		WIN32API DWORD FormatMessageW(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBuffer, DWORD nSize, va_list* Arguments);

		WIN32API int GetSystemMetrics(int nIndex);

		WIN32API int MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

		WIN32API HMODULE GetModuleHandleW(LPCWSTR lpModuleName);
		WIN32API HCURSOR LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName);

		WIN32API HANDLE CreateEventW(SECURITY_ATTRIBUTES* lpEventAttributes, bool bManualReset, bool bInitialState, LPCWSTR lpName);
		WIN32API DWORD  WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, bool bAlertable);

		WIN32API ATOM RegisterClassExW(const WNDCLASSEXW* wndClass);
		WIN32API bool UnregisterClassW(LPCWSTR lpClassName, HINSTANCE hInstance);

		WIN32API bool GetMonitorInfoW(HMONITOR hMonitor, MONITORINFO* lpmi);

		WIN32API HWND     CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
		WIN32API bool     DestroyWindow(HWND hWnd);
		WIN32API bool     ShowWindow(HWND hWnd, int nCmdShow);
		WIN32API HMONITOR MonitorFromWindow(HWND hWnd, DWORD dwFlags);
		WIN32API bool     SetPropW(HWND hWnd, LPCWSTR lpString, HANDLE hData);
		WIN32API HANDLE   GetPropW(HWND hWnd, LPCWSTR lpString);
		WIN32API LONG     SetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong);
		WIN32API LONG     GetWindowLongW(HWND hWnd, int nIndex);
		WIN32API bool     SetWindowTextW(HWND hWnd, LPCWSTR lpString);
		WIN32API bool     SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
		WIN32API bool     GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT* lpwndpl);
		WIN32API bool     SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);
		WIN32API bool     PeekMessageW(MSG* lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
		WIN32API bool     TranslateMessage(const MSG* lpMsg);
		WIN32API LRESULT  DispatchMessageW(const MSG* lpMsg);
		WIN32API LRESULT  DefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	}

	static constexpr DWORD FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x0100;
	static constexpr DWORD FORMAT_MESSAGE_FROM_SYSTEM     = 0x1000;

	static constexpr DWORD LANG_NEUTRAL    = 0x00;
	static constexpr DWORD SUBLANG_DEFAULT = 0x01;

	static constexpr DWORD INFINITE = 0xFFFF'FFFF;

	static LPCWSTR const IDC_ARROW = reinterpret_cast<LPCWSTR const>(32512);

	static constexpr int SM_CXSCREEN = 0;
	static constexpr int SM_CYSCREEN = 1;

	static constexpr DWORD MONITOR_DEFAULTTOPRIMARY = 0x0000'0001;

	static constexpr int GWL_STYLE = -16;

	static constexpr UINT SWP_NOSIZE         = 0x0001;
	static constexpr UINT SWP_NOMOVE         = 0x0002;
	static constexpr UINT SWP_NOZORDER       = 0x0004;
	static constexpr UINT SWP_FRAMECHANGED   = 0x0020;
	static constexpr UINT SWP_NOOWNERZORDER  = 0x0200;
	static constexpr UINT SWP_NOSENDCHANGING = 0x0400;

	static constexpr UINT CS_VREDRAW = 0x0001;
	static constexpr UINT CS_HREDRAW = 0x0002;

	static constexpr DWORD WS_OVERLAPPED       = 0x0000'0000;
	static constexpr DWORD WS_MAXIMIZEBOX      = 0x0001'0000;
	static constexpr DWORD WS_MINIMIZEBOX      = 0x0002'0000;
	static constexpr DWORD WS_THICKFRAME       = 0x0004'0000;
	static constexpr DWORD WS_SYSMENU          = 0x0008'0000;
	static constexpr DWORD WS_CAPTION          = 0x00C0'0000;
	static constexpr DWORD WS_OVERLAPPEDWINDOW = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	static constexpr int SW_HIDE     = 0;
	static constexpr int SW_NORMAL   = 1;
	static constexpr int SW_MAXIMIZE = 3;
	static constexpr int SW_MINIMIZE = 6;

	static constexpr UINT MB_OK = 0x0000;

	static constexpr UINT PM_REMOVE = 0x0001;

	static constexpr UINT WM_MOVE          = 0x0003;
	static constexpr UINT WM_SIZE          = 0x0005;
	static constexpr UINT WM_CLOSE         = 0x0010;
	static constexpr UINT WM_QUIT          = 0x0012;
	static constexpr UINT WM_KEYDOWN       = 0x0100;
	static constexpr UINT WM_KEYUP         = 0x0101;
	static constexpr UINT WM_SYSKEYDOWN    = 0x0104;
	static constexpr UINT WM_SYSKEYUP      = 0x0105;
	static constexpr UINT WM_SYSCOMMAND    = 0x0112;
	static constexpr UINT WM_ENTERSIZEMOVE = 0x0231;
	static constexpr UINT WM_EXITSIZEMOVE  = 0x0232;

	static constexpr WPARAM SIZE_RESTORED  = 0;
	static constexpr WPARAM SIZE_MINIMIZED = 1;
	static constexpr WPARAM SIZE_MAXIMIZED = 2;

	static constexpr WPARAM SC_SCREENSAVE   = 0xF140;
	static constexpr WPARAM SC_MONITORPOWER = 0xF170;

	static constexpr WPARAM VK_F9  = 0x78;
	static constexpr WPARAM VK_F10 = 0x79;
	static constexpr WPARAM VK_F11 = 0x7A;
	static constexpr WPARAM VK_F12 = 0x7B;

	constexpr std::uint32_t LOWORD(LPARAM l)
	{
		return static_cast<std::uint32_t>(l & 0xFFFF);
	}

	constexpr std::uint32_t HIWORD(LPARAM l)
	{
		return static_cast<std::uint32_t>((l >> 16) & 0xFFFF);
	}

	constexpr DWORD MAKELANGID(DWORD p, DWORD s)
	{
		return s << 10 | p;
	}
} // namespace Brainfryer::Windows