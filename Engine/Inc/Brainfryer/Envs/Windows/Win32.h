#pragma once

#include "Brainfryer/Utils/Core.h"
#include "Types.h"

#if BUILD_IS_SYSTEM_WINDOWS
#define WIN32API __declspec(dllimport)
#else
#define WIN32API
#endif

namespace Brainfryer::Windows
{
	using namespace Types;

	using WNDPROC = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);
	using DLGPROC = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);

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

	struct DLGTEMPLATE
	{
		DWORD style;
		DWORD dwExtendedStyle;
		WORD  cdit;
		short x;
		short y;
		short cx;
		short cy;
	};

	struct DLGITEMTEMPLATE
	{
		DWORD style;
		DWORD dwExtendedStyle;
		short x;
		short y;
		short cx;
		short cy;
		WORD  id;
	};

	extern "C"
	{
		WIN32API DWORD GetLastError();

		WIN32API int lstrlenW(LPCWSTR lpString);

		WIN32API HLOCAL LocalAlloc(UINT uFlags, SIZE_T uBytes);
		WIN32API HLOCAL LocalFree(HLOCAL hMem);

		WIN32API DWORD GetLastError();
		WIN32API DWORD FormatMessageW(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBuffer, DWORD nSize, va_list* Arguments);

		WIN32API int GetSystemMetrics(int nIndex);

		WIN32API INT_PTR DialogBoxIndirectParamW(HINSTANCE hInstance, const DLGTEMPLATE* lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
		WIN32API LRESULT DefDlgProcW(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
		WIN32API bool    EndDialog(HWND hDlg, INT_PTR nResult);
		WIN32API int     MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

		WIN32API HANDLE  GetCurrentProcess();
		WIN32API HMODULE GetModuleHandleW(LPCWSTR lpModuleName);
		WIN32API void*   GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
		WIN32API HCURSOR LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName);

		WIN32API BOOL VirtualProtectEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD* lpflOldProtect);
		WIN32API BOOL FlushInstructionCache(HANDLE hProcess, LPCVOID lpBaseAddress, SIZE_T dwSize);

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
	static constexpr DWORD FORMAT_MESSAGE_IGNORE_INSERTS  = 0x0200;
	static constexpr DWORD FORMAT_MESSAGE_FROM_SYSTEM     = 0x1000;

	static constexpr DWORD LANG_NEUTRAL    = 0x00;
	static constexpr DWORD SUBLANG_DEFAULT = 0x01;

	static constexpr DWORD PAGE_EXECUTE           = 0x0000'0010;
	static constexpr DWORD PAGE_EXECUTE_READ      = 0x0000'0020;
	static constexpr DWORD PAGE_EXECUTE_READWRITE = 0x0000'0040;
	static constexpr DWORD PAGE_EXECUTE_WRITECOPY = 0x0000'0080;
	static constexpr DWORD PAGE_NOACCESS          = 0x0000'0001;
	static constexpr DWORD PAGE_READONLY          = 0x0000'0002;
	static constexpr DWORD PAGE_READWRITE         = 0x0000'0004;
	static constexpr DWORD PAGE_WRITECOPY         = 0x0000'0008;
	static constexpr DWORD PAGE_TARGETS_INVALID   = 0x4000'0000;
	static constexpr DWORD PAGE_TARGETS_NO_UPDATE = 0x4000'0000;

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

	static constexpr DWORD WS_TILED            = 0x0000'0000;
	static constexpr DWORD WS_OVERLAPPED       = 0x0000'0000;
	static constexpr DWORD WS_TABSTOP          = 0x0001'0000;
	static constexpr DWORD WS_MAXIMIZEBOX      = 0x0001'0000;
	static constexpr DWORD WS_GROUP            = 0x0002'0000;
	static constexpr DWORD WS_MINIMIZEBOX      = 0x0002'0000;
	static constexpr DWORD WS_SIZEBOX          = 0x0004'0000;
	static constexpr DWORD WS_THICKFRAME       = 0x0004'0000;
	static constexpr DWORD WS_SYSMENU          = 0x0008'0000;
	static constexpr DWORD WS_HSCROLL          = 0x0010'0000;
	static constexpr DWORD WS_VSCROLL          = 0x0020'0000;
	static constexpr DWORD WS_DLGFRAME         = 0x0040'0000;
	static constexpr DWORD WS_BORDER           = 0x0080'0000;
	static constexpr DWORD WS_CAPTION          = 0x00C0'0000;
	static constexpr DWORD WS_MAXIMIZE         = 0x0100'0000;
	static constexpr DWORD WS_CLIPCHILDREN     = 0x0200'0000;
	static constexpr DWORD WS_CLIPSIBLINGS     = 0x0400'0000;
	static constexpr DWORD WS_DISABLED         = 0x0800'0000;
	static constexpr DWORD WS_VISIBLE          = 0x1000'0000;
	static constexpr DWORD WS_ICONIC           = 0x2000'0000;
	static constexpr DWORD WS_MINIMIZE         = 0x2000'0000;
	static constexpr DWORD WS_CHILD            = 0x4000'0000;
	static constexpr DWORD WS_CHILDWINDOW      = 0x4000'0000;
	static constexpr DWORD WS_POPUP            = 0x8000'0000;
	static constexpr DWORD WS_POPUPWINDOW      = WS_POPUP | WS_BORDER | WS_SYSMENU;
	static constexpr DWORD WS_OVERLAPPEDWINDOW = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	static constexpr DWORD WS_TILEDWINDOW      = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	static constexpr DWORD DS_ABSALIGN      = 0x0001;
	static constexpr DWORD DS_SYSMODAL      = 0x0002;
	static constexpr DWORD DS_LOCALEDIT     = 0x0020;
	static constexpr DWORD DS_SETFONT       = 0x0040;
	static constexpr DWORD DS_MODALFRAME    = 0x0080;
	static constexpr DWORD DS_NOIDLEMSG     = 0x0100;
	static constexpr DWORD DS_SETFOREGROUND = 0x0200;
	static constexpr DWORD DS_3DLOOK        = 0x0004;
	static constexpr DWORD DS_FIXEDSYS      = 0x0008;
	static constexpr DWORD DS_NOFAILCREATE  = 0x0010;
	static constexpr DWORD DS_CONTROL       = 0x0400;
	static constexpr DWORD DS_CENTER        = 0x0800;
	static constexpr DWORD DS_CENTERMOUSE   = 0x1000;
	static constexpr DWORD DS_CONTEXTHELP   = 0x2000;
	static constexpr DWORD DS_SHELLFONT     = DS_SETFONT | DS_FIXEDSYS;

	static constexpr DWORD BS_PUSHBUTTON      = 0x00000000;
	static constexpr DWORD BS_DEFPUSHBUTTON   = 0x00000001;
	static constexpr DWORD BS_CHECKBOX        = 0x00000002;
	static constexpr DWORD BS_AUTOCHECKBOX    = 0x00000003;
	static constexpr DWORD BS_RADIOBUTTON     = 0x00000004;
	static constexpr DWORD BS_3STATE          = 0x00000005;
	static constexpr DWORD BS_AUTO3STATE      = 0x00000006;
	static constexpr DWORD BS_GROUPBOX        = 0x00000007;
	static constexpr DWORD BS_USERBUTTON      = 0x00000008;
	static constexpr DWORD BS_AUTORADIOBUTTON = 0x00000009;
	static constexpr DWORD BS_PUSHBOX         = 0x0000000A;
	static constexpr DWORD BS_OWNERDRAW       = 0x0000000B;
	static constexpr DWORD BS_TYPEMASK        = 0x0000000F;
	static constexpr DWORD BS_LEFTTEXT        = 0x00000020;
	static constexpr DWORD BS_TEXT            = 0x00000000;
	static constexpr DWORD BS_ICON            = 0x00000040;
	static constexpr DWORD BS_BITMAP          = 0x00000080;
	static constexpr DWORD BS_LEFT            = 0x00000100;
	static constexpr DWORD BS_RIGHT           = 0x00000200;
	static constexpr DWORD BS_CENTER          = 0x00000300;
	static constexpr DWORD BS_TOP             = 0x00000400;
	static constexpr DWORD BS_BOTTOM          = 0x00000800;
	static constexpr DWORD BS_VCENTER         = 0x00000C00;
	static constexpr DWORD BS_PUSHLIKE        = 0x00001000;
	static constexpr DWORD BS_MULTILINE       = 0x00002000;
	static constexpr DWORD BS_NOTIFY          = 0x00004000;
	static constexpr DWORD BS_FLAT            = 0x00008000;
	static constexpr DWORD BS_RIGHTBUTTON     = BS_LEFTTEXT;

	static constexpr DWORD SS_LEFT            = 0x00000000;
	static constexpr DWORD SS_CENTER          = 0x00000001;
	static constexpr DWORD SS_RIGHT           = 0x00000002;
	static constexpr DWORD SS_ICON            = 0x00000003;
	static constexpr DWORD SS_BLACKRECT       = 0x00000004;
	static constexpr DWORD SS_GRAYRECT        = 0x00000005;
	static constexpr DWORD SS_WHITERECT       = 0x00000006;
	static constexpr DWORD SS_BLACKFRAME      = 0x00000007;
	static constexpr DWORD SS_GRAYFRAME       = 0x00000008;
	static constexpr DWORD SS_WHITEFRAME      = 0x00000009;
	static constexpr DWORD SS_USERITEM        = 0x0000000A;
	static constexpr DWORD SS_SIMPLE          = 0x0000000B;
	static constexpr DWORD SS_LEFTNOWORDWRAP  = 0x0000000C;
	static constexpr DWORD SS_OWNERDRAW       = 0x0000000D;
	static constexpr DWORD SS_BITMAP          = 0x0000000E;
	static constexpr DWORD SS_ENHMETAFILE     = 0x0000000F;
	static constexpr DWORD SS_ETCHEDHORZ      = 0x00000010;
	static constexpr DWORD SS_ETCHEDVERT      = 0x00000011;
	static constexpr DWORD SS_ETCHEDFRAME     = 0x00000012;
	static constexpr DWORD SS_TYPEMASK        = 0x0000001F;
	static constexpr DWORD SS_REALSIZECONTROL = 0x00000040;
	static constexpr DWORD SS_NOPREFIX        = 0x00000080;
	static constexpr DWORD SS_NOTIFY          = 0x00000100;
	static constexpr DWORD SS_CENTERIMAGE     = 0x00000200;
	static constexpr DWORD SS_RIGHTJUST       = 0x00000400;
	static constexpr DWORD SS_REALSIZEIMAGE   = 0x00000800;
	static constexpr DWORD SS_SUNKEN          = 0x00001000;
	static constexpr DWORD SS_EDITCONTROL     = 0x00002000;
	static constexpr DWORD SS_ENDELLIPSIS     = 0x00004000;
	static constexpr DWORD SS_PATHELLIPSIS    = 0x00008000;
	static constexpr DWORD SS_WORDELLIPSIS    = 0x0000C000;
	static constexpr DWORD SS_ELLIPSISMASK    = 0x0000C000;

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
	static constexpr UINT WM_SETFONT       = 0x0030;
	static constexpr UINT WM_KEYDOWN       = 0x0100;
	static constexpr UINT WM_KEYUP         = 0x0101;
	static constexpr UINT WM_SYSKEYDOWN    = 0x0104;
	static constexpr UINT WM_SYSKEYUP      = 0x0105;
	static constexpr UINT WM_INITDIALOG    = 0x0110;
	static constexpr UINT WM_COMMAND       = 0x0111;
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