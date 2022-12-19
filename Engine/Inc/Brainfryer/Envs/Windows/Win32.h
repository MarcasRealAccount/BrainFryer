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

	using HRESULT = std::int32_t;

	using WNDPROC         = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);
	using DLGPROC         = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);
	using MONITORENUMPROC = BOOL (*)(HMONITOR, HDC, RECT*, LPARAM);
	using TIMERPROC       = void (*)(HWND, UINT, UINT_PTR, DWORD);

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

	using RAWINPUTHEADER = struct tagRAWINPUTHEADER
	{
		DWORD  dwType;
		DWORD  dwSize;
		HANDLE hDevice;
		WPARAM wParam;
	};

	using RAWMOUSE = struct tagRAWMOUSE
	{
		USHORT usFlags;
		union
		{
			ULONG ulButtons;
			struct
			{
				USHORT usButtonFlags;
				USHORT usButtonData;
			} DUMMYSTRUCTNAME;
		} DUMMYUNIONNAME;
		ULONG ulRawButtons;
		LONG  lLastX;
		LONG  lLastY;
		ULONG ulExtraInformation;
	};

	using RAWKEYBOARD = struct tagRAWKEYBOARD
	{
		USHORT MakeCode;
		USHORT Flags;
		USHORT Reserved;
		USHORT VKey;
		UINT   Message;
		ULONG  ExtraInformation;
	};

	using RAWHID = struct tagRAWHID
	{
		DWORD dwSizeHid;
		DWORD dwCount;
		BYTE  bRawData[1];
	};

	using RAWINPUT = struct tagRAWINPUT
	{
		RAWINPUTHEADER header;
		union
		{
			RAWMOUSE    mouse;
			RAWKEYBOARD keyboard;
			RAWHID      hid;
		} data;
	};

	using TRACKMOUSEEVENT = struct tagTRACKMOUSEEVENT
	{
		DWORD cbSize;
		DWORD dwFlags;
		HWND  hwndTrack;
		DWORD dwHoverTime;
	};

	using COLORREF = DWORD;

	using SE_OBJECT_TYPE = enum _SE_OBJECT_TYPE {
		SE_UNKNOWN_OBJECT_TYPE,
		SE_FILE_OBJECT,
		SE_SERVICE,
		SE_PRINTER,
		SE_REGISTRY_KEY,
		SE_LMSHARE,
		SE_KERNEL_OBJECT,
		SE_WINDOW_OBJECT,
		SE_DS_OBJECT,
		SE_DS_OBJECT_ALL,
		SE_PROVIDER_DEFINED_OBJECT,
		SE_WMIGUID_OBJECT,
		SE_REGISTRY_WOW64_32KEY,
		SE_REGISTRY_WOW64_64KEY
	};

	using SECURITY_INFORMATION = DWORD;
	using PSID                 = void*;
	using PACL                 = void*;
	using PSECURITY_DESCRIPTOR = void*;

	using ACCESS_MASK = DWORD;

	using GENERIC_MAPPING = struct _GENERIC_MAPPING
	{
		ACCESS_MASK GenericRead;
		ACCESS_MASK GenericWrite;
		ACCESS_MASK GenericExecute;
		ACCESS_MASK GenericAll;
	};

	using LUID_AND_ATTRIBUTES = struct _LUID_AND_ATTRIBUTES
	{
		LUID  Luid;
		DWORD Attributes;
	};

	using PRIVILEGE_SET = struct _PRIVILEGE_SET
	{
		DWORD               PrivilegeCount;
		DWORD               Control;
		LUID_AND_ATTRIBUTES Privilege[1];
	};

	using EXCEPTION_RECORD = struct _EXCEPTION_RECORD
	{
		DWORD                     ExceptionCode;
		DWORD                     ExceptionFlags;
		struct _EXCEPTION_RECORD* ExceptionRecord;
		void*                     ExceptionAddress;
		DWORD                     NumberParameters;
		ULONG_PTR                 ExceptionInformation[15];
	};


	using M128A = struct alignas(16) _M128A
	{
		ULONGLONG Low;
		LONGLONG  High;
	};

	using XSAVE_FORMAT = struct alignas(16) _XSAVE_FORMAT
	{
		WORD  ControlWord;
		WORD  StatusWord;
		BYTE  TagWord;
		BYTE  Reserved1;
		WORD  ErrorOpcode;
		DWORD ErrorOffset;
		WORD  ErrorSelector;
		WORD  Reserved2;
		DWORD DataOffset;
		WORD  DataSelector;
		WORD  Reserved3;
		DWORD MxCsr;
		DWORD MxCsr_Mask;
		M128A FloatRegisters[8];
		M128A XmmRegisters[16];
		BYTE  Reserved4[96];
	};
	using XMM_SAVE_AREA32 = XSAVE_FORMAT;

	using CONTEXT = struct alignas(16) _CONTEXT
	{
		DWORD64 P1Home;
		DWORD64 P2Home;
		DWORD64 P3Home;
		DWORD64 P4Home;
		DWORD64 P5Home;
		DWORD64 P6Home;
		DWORD   ContextFlags;
		DWORD   MxCsr;
		WORD    SegCs;
		WORD    SegDs;
		WORD    SegEs;
		WORD    SegFs;
		WORD    SegGs;
		WORD    SegSs;
		DWORD   EFlags;
		DWORD64 Dr0;
		DWORD64 Dr1;
		DWORD64 Dr2;
		DWORD64 Dr3;
		DWORD64 Dr6;
		DWORD64 Dr7;
		DWORD64 Rax;
		DWORD64 Rcx;
		DWORD64 Rdx;
		DWORD64 Rbx;
		DWORD64 Rsp;
		DWORD64 Rbp;
		DWORD64 Rsi;
		DWORD64 Rdi;
		DWORD64 R8;
		DWORD64 R9;
		DWORD64 R10;
		DWORD64 R11;
		DWORD64 R12;
		DWORD64 R13;
		DWORD64 R14;
		DWORD64 R15;
		DWORD64 Rip;
		union
		{
			XMM_SAVE_AREA32 FltSave;
			struct
			{
				M128A Header[2];
				M128A Legacy[8];
				M128A Xmm0;
				M128A Xmm1;
				M128A Xmm2;
				M128A Xmm3;
				M128A Xmm4;
				M128A Xmm5;
				M128A Xmm6;
				M128A Xmm7;
				M128A Xmm8;
				M128A Xmm9;
				M128A Xmm10;
				M128A Xmm11;
				M128A Xmm12;
				M128A Xmm13;
				M128A Xmm14;
				M128A Xmm15;
			} DUMMYSTRUCTNAME;
		} DUMMYUNIONNAME;
		M128A   VectorRegister[26];
		DWORD64 VectorControl;
		DWORD64 DebugControl;
		DWORD64 LastBranchToRip;
		DWORD64 LastBranchFromRip;
		DWORD64 LastExceptionToRip;
		DWORD64 LastExceptionFromRip;
	};

	using EXCEPTION_POINTERS = struct _EXCEPTION_POINTERS
	{
		EXCEPTION_RECORD* ExceptionRecord;
		CONTEXT*          ContextRecord;
	};

	using WINDOWPOS = struct tagWINDOWPOS
	{
		HWND hwnd;
		HWND hwndInsertAfter;
		int  x;
		int  y;
		int  cx;
		int  cy;
		UINT flags;
	};

	using NCCALCSIZE_PARAMS = struct tagNCCALCSIZE_PARAMS
	{
		RECT       rgrc[3];
		WINDOWPOS* lppos;
	};

	using MARGINS = struct _MARGINS
	{
		int cxLeftWidth;
		int cxRightWidth;
		int cyTopHeight;
		int cyBottomHeight;
	};

	using PVECTORED_EXCEPTION_HANDLER = LONG (*)(EXCEPTION_POINTERS* ExceptionInfo);

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
		WIN32API BOOL    EndDialog(HWND hDlg, INT_PTR nResult);
		WIN32API int     MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

		WIN32API HANDLE  GetCurrentProcess();
		WIN32API HMODULE GetModuleHandleW(LPCWSTR lpModuleName);
		WIN32API void*   GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
		WIN32API HCURSOR LoadCursorW(HINSTANCE hInstance, LPCWSTR lpCursorName);
		WIN32API HCURSOR SetCursor(HCURSOR hCursor);
		WIN32API HMODULE LoadLibraryW(LPCWSTR lpLibFileName);
		WIN32API BOOL    FreeLibrary(HMODULE hLibModule);
		WIN32API DWORD   GetSecurityInfo(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl, PSECURITY_DESCRIPTOR* ppSecurityDescriptor);
		WIN32API BOOL    AccessCheck(PSECURITY_DESCRIPTOR pSecurityDescriptor, HANDLE ClientToken, DWORD DesiredAccess, GENERIC_MAPPING* GenericMapping, PRIVILEGE_SET* PrivilegeSet, DWORD* PrivilegeSetLength, DWORD* GrantedAccess, BOOL* AccessStatus);

		WIN32API BOOL VirtualProtectEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD* lpflOldProtect);
		WIN32API BOOL FlushInstructionCache(HANDLE hProcess, LPCVOID lpBaseAddress, SIZE_T dwSize);

		WIN32API HANDLE CreateEventW(SECURITY_ATTRIBUTES* lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);
		WIN32API DWORD  WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable);

		WIN32API ATOM RegisterClassExW(const WNDCLASSEXW* wndClass);
		WIN32API BOOL UnregisterClassW(LPCWSTR lpClassName, HINSTANCE hInstance);

		WIN32API BOOL GetMonitorInfoW(HMONITOR hMonitor, MONITORINFO* lpmi);
		WIN32API BOOL EnumDisplayMonitors(HDC hdc, const RECT* lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData);

		WIN32API BOOL ScreenToClient(HWND hWnd, POINT* lpPoint);
		WIN32API BOOL ClientToScreen(HWND hWnd, POINT* lpPoint);
		WIN32API BOOL SetCursorPos(int X, int Y);
		WIN32API BOOL GetCursorPos(POINT* lpPoint);
		WIN32API HWND WindowFromPoint(POINT Point);

		WIN32API HWND     CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
		WIN32API BOOL     DestroyWindow(HWND hWnd);
		WIN32API BOOL     ShowWindow(HWND hWnd, int nCmdShow);
		WIN32API HMONITOR MonitorFromWindow(HWND hWnd, DWORD dwFlags);
		WIN32API BOOL     SetPropW(HWND hWnd, LPCWSTR lpString, HANDLE hData);
		WIN32API HANDLE   GetPropW(HWND hWnd, LPCWSTR lpString);
		WIN32API LONG     SetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong);
		WIN32API LONG     GetWindowLongW(HWND hWnd, int nIndex);
		WIN32API BOOL     SetWindowTextW(HWND hWnd, LPCWSTR lpString);
		WIN32API BOOL     SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
		WIN32API BOOL     GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT* lpwndpl);
		WIN32API BOOL     SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);
		WIN32API BOOL     PeekMessageW(MSG* lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
		WIN32API LONG     GetMessageTime();
		WIN32API BOOL     TranslateMessage(const MSG* lpMsg);
		WIN32API LRESULT  DispatchMessageW(const MSG* lpMsg);
		WIN32API LRESULT  DefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		WIN32API BOOL     BringWindowToTop(HWND hWnd);
		WIN32API BOOL     SetForegroundWindow(HWND hWnd);
		WIN32API HWND     SetFocus(HWND hWnd);
		WIN32API BOOL     SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
		WIN32API UINT_PTR SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
		WIN32API BOOL     KillTimer(HWND hWnd, UINT_PTR nIDEvent);

		WIN32API UINT  MapVirtualKeyW(UINT uCode, UINT uMapType);
		WIN32API UINT  GetRawInputData(RAWINPUT* hRawInput, UINT uiCommand, LPVOID pData, UINT* pcbSize, UINT cbSizeHeader);
		WIN32API BOOL  TrackMouseEvent(TRACKMOUSEEVENT* lpEventTrack);
		WIN32API SHORT GetKeyState(int nKeycode);

		WIN32API void* AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler);
		WIN32API ULONG RemoveVectoredExceptionHandler(void* Handle);

		WIN32API HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset);
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

	static LPCWSTR const IDC_ARROW    = reinterpret_cast<LPCWSTR const>(32512);
	static LPCWSTR const IDC_HAND     = reinterpret_cast<LPCWSTR const>(32649);
	static LPCWSTR const IDC_IBEAM    = reinterpret_cast<LPCWSTR const>(32513);
	static LPCWSTR const IDC_NO       = reinterpret_cast<LPCWSTR const>(32648);
	static LPCWSTR const IDC_SIZE     = reinterpret_cast<LPCWSTR const>(32640);
	static LPCWSTR const IDC_SIZEALL  = reinterpret_cast<LPCWSTR const>(32646);
	static LPCWSTR const IDC_SIZENESW = reinterpret_cast<LPCWSTR const>(32643);
	static LPCWSTR const IDC_SIZENS   = reinterpret_cast<LPCWSTR const>(32645);
	static LPCWSTR const IDC_SIZENWSE = reinterpret_cast<LPCWSTR const>(32642);
	static LPCWSTR const IDC_SIZEWE   = reinterpret_cast<LPCWSTR const>(32644);

	static constexpr DWORD MONITOR_DEFAULTTONULL    = 0x0000'0000;
	static constexpr DWORD MONITOR_DEFAULTTOPRIMARY = 0x0000'0001;
	static constexpr DWORD MONITOR_DEFAULTTONEAREST = 0x0000'0002;

	static constexpr int GWL_USERDATA  = -21;
	static constexpr int GWL_EXSTYLE   = -20;
	static constexpr int GWL_STYLE     = -16;
	static constexpr int GWL_ID        = -12;
	static constexpr int GWL_HINSTANCE = -6;
	static constexpr int GWL_WNDPROC   = -4;

	static constexpr DWORD SWP_NOSIZE         = 0x0001;
	static constexpr DWORD SWP_NOMOVE         = 0x0002;
	static constexpr DWORD SWP_NOZORDER       = 0x0004;
	static constexpr DWORD SWP_NOREDRAW       = 0x0008;
	static constexpr DWORD SWP_NOACTIVATE     = 0x0010;
	static constexpr DWORD SWP_DRAWFRAME      = 0x0020;
	static constexpr DWORD SWP_FRAMECHANGED   = 0x0020;
	static constexpr DWORD SWP_SHOWWINDOW     = 0x0040;
	static constexpr DWORD SWP_HIDEWINDOW     = 0x0080;
	static constexpr DWORD SWP_NOCOPYBITS     = 0x0100;
	static constexpr DWORD SWP_NOOWNERZORDER  = 0x0200;
	static constexpr DWORD SWP_NOREPOSITION   = 0x0200;
	static constexpr DWORD SWP_NOSENDCHANGING = 0x0400;
	static constexpr DWORD SWP_DEFERERASE     = 0x2000;
	static constexpr DWORD SWP_ASYNCWINDOWPOS = 0x4000;

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

	static constexpr DWORD WS_EX_ACCEPTFILES         = 0x00000010;
	static constexpr DWORD WS_EX_APPWINDOW           = 0x00040000;
	static constexpr DWORD WS_EX_CLIENTEDGE          = 0x00000200;
	static constexpr DWORD WS_EX_COMPOSITED          = 0x02000000;
	static constexpr DWORD WS_EX_CONTEXTHELP         = 0x00000400;
	static constexpr DWORD WS_EX_CONTROLPARENT       = 0x00010000;
	static constexpr DWORD WS_EX_DLGMODALFRAME       = 0x00000001;
	static constexpr DWORD WS_EX_LAYERED             = 0x00080000;
	static constexpr DWORD WS_EX_LAYOUTRTL           = 0x00400000;
	static constexpr DWORD WS_EX_LEFT                = 0x00000000;
	static constexpr DWORD WS_EX_LEFTSCROLLBAR       = 0x00004000;
	static constexpr DWORD WS_EX_LTRREADING          = 0x00000000;
	static constexpr DWORD WS_EX_MDICHILD            = 0x00000040;
	static constexpr DWORD WS_EX_NOACTIVATE          = 0x08000000;
	static constexpr DWORD WS_EX_NOINHERITLAYOUT     = 0x00100000;
	static constexpr DWORD WS_EX_NOPARENTNOTIFY      = 0x00000004;
	static constexpr DWORD WS_EX_NOREDIRECTIONBITMAP = 0x00200000;
	static constexpr DWORD WS_EX_RIGHT               = 0x00001000;
	static constexpr DWORD WS_EX_RIGHTSCROLLBAR      = 0x00000000;
	static constexpr DWORD WS_EX_RTLREADING          = 0x00002000;
	static constexpr DWORD WS_EX_STATICEDGE          = 0x00020000;
	static constexpr DWORD WS_EX_TOOLWINDOW          = 0x00000080;
	static constexpr DWORD WS_EX_TOPMOST             = 0x00000008;
	static constexpr DWORD WS_EX_TRANSPARENT         = 0x00000020;
	static constexpr DWORD WS_EX_WINDOWEDGE          = 0x00000100;
	static constexpr DWORD WS_EX_OVERLAPPEDWINDOW    = (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE);
	static constexpr DWORD WS_EX_PALETTEWINDOW       = (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

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

	static constexpr int SW_HIDE               = 0;
	static constexpr int SW_NORMAL             = 1;
	static constexpr int SW_SHOWMINIMIZED      = 2;
	static constexpr int SW_MAXIMIZE           = 3;
	static constexpr int SW_SHOWNOACTIVATE     = 4;
	static constexpr int SW_SHOW               = 5;
	static constexpr int SW_MINIMIZE           = 6;
	static constexpr int SW_MINIMIZENOACTIVATE = 7;
	static constexpr int SW_SHOWNA             = 8;
	static constexpr int SW_RESTORE            = 9;
	static constexpr int SW_SHOWDEFAULT        = 10;
	static constexpr int SW_FORCEMINIMIZE      = 11;

	static constexpr UINT MB_OK = 0x0000;

	static constexpr UINT PM_NOREMOVE = 0x0000;
	static constexpr UINT PM_REMOVE   = 0x0001;

	static constexpr UINT WM_MOVE          = 0x0003;
	static constexpr UINT WM_SIZE          = 0x0005;
	static constexpr UINT WM_SETFOCUS      = 0x0007;
	static constexpr UINT WM_KILLFOCUS     = 0x0008;
	static constexpr UINT WM_CLOSE         = 0x0010;
	static constexpr UINT WM_QUIT          = 0x0012;
	static constexpr UINT WM_SETCURSOR     = 0x0020;
	static constexpr UINT WM_SETFONT       = 0x0030;
	static constexpr UINT WM_NCCALCSIZE    = 0x0083;
	static constexpr UINT WM_NCHITTEST     = 0x0084;
	static constexpr UINT WM_INPUT         = 0x00FF;
	static constexpr UINT WM_KEYDOWN       = 0x0100;
	static constexpr UINT WM_KEYUP         = 0x0101;
	static constexpr UINT WM_CHAR          = 0x0102;
	static constexpr UINT WM_SYSKEYDOWN    = 0x0104;
	static constexpr UINT WM_SYSKEYUP      = 0x0105;
	static constexpr UINT WM_SYSCHAR       = 0x0106;
	static constexpr UINT WM_UNICHAR       = 0x0109;
	static constexpr UINT WM_INITDIALOG    = 0x0110;
	static constexpr UINT WM_COMMAND       = 0x0111;
	static constexpr UINT WM_SYSCOMMAND    = 0x0112;
	static constexpr UINT WM_TIMER         = 0x0113;
	static constexpr UINT WM_MOUSEMOVE     = 0x0200;
	static constexpr UINT WM_LBUTTONDOWN   = 0x0201;
	static constexpr UINT WM_LBUTTONUP     = 0x0202;
	static constexpr UINT WM_RBUTTONDOWN   = 0x0204;
	static constexpr UINT WM_RBUTTONUP     = 0x0205;
	static constexpr UINT WM_MBUTTONDOWN   = 0x0207;
	static constexpr UINT WM_MBUTTONUP     = 0x0208;
	static constexpr UINT WM_MOUSEWHEEL    = 0x020A;
	static constexpr UINT WM_XBUTTONDOWN   = 0x020B;
	static constexpr UINT WM_XBUTTONUP     = 0x020C;
	static constexpr UINT WM_MOUSEHWHEEL   = 0x020E;
	static constexpr UINT WM_ENTERSIZEMOVE = 0x0231;
	static constexpr UINT WM_EXITSIZEMOVE  = 0x0232;
	static constexpr UINT WM_MOUSELEAVE    = 0x02A3;
	static constexpr UINT WM_DPICHANGED    = 0x02E0;

	static constexpr UINT UNICODE_NOCHAR = 0xFFFF;

	static constexpr WPARAM SIZE_RESTORED  = 0;
	static constexpr WPARAM SIZE_MINIMIZED = 1;
	static constexpr WPARAM SIZE_MAXIMIZED = 2;

	static constexpr WPARAM SCF_ISSECURE    = 0x0001;
	static constexpr WPARAM SC_SIZE         = 0xF000;
	static constexpr WPARAM SC_MOVE         = 0xF010;
	static constexpr WPARAM SC_MINIMIZE     = 0xF020;
	static constexpr WPARAM SC_MAXIMIZE     = 0xF030;
	static constexpr WPARAM SC_NEXTWINDOW   = 0xF040;
	static constexpr WPARAM SC_PREVWINDOW   = 0xF050;
	static constexpr WPARAM SC_CLOSE        = 0xF060;
	static constexpr WPARAM SC_VSCROLL      = 0xF070;
	static constexpr WPARAM SC_HSCROLL      = 0xF080;
	static constexpr WPARAM SC_MOUSEMENU    = 0xF090;
	static constexpr WPARAM SC_KEYMENU      = 0xF100;
	static constexpr WPARAM SC_RESTORE      = 0xF120;
	static constexpr WPARAM SC_TASKLIST     = 0xF130;
	static constexpr WPARAM SC_SCREENSAVE   = 0xF140;
	static constexpr WPARAM SC_HOTKEY       = 0xF150;
	static constexpr WPARAM SC_DEFAULT      = 0xF160;
	static constexpr WPARAM SC_MONITORPOWER = 0xF170;
	static constexpr WPARAM SC_CONTEXTHELP  = 0xF180;

	static constexpr DWORD KF_EXTENDED = 0x0100;
	static constexpr DWORD KF_DLGMODE  = 0x0800;
	static constexpr DWORD KF_MENUMODE = 0x1000;
	static constexpr DWORD KF_ALTDOWN  = 0x2000;
	static constexpr DWORD KF_REPEAT   = 0x4000;
	static constexpr DWORD KF_UP       = 0x8000;

	static constexpr WPARAM VK_SHIFT      = 0x10;
	static constexpr WPARAM VK_CONTROL    = 0x11;
	static constexpr WPARAM VK_MENU       = 0x12;
	static constexpr WPARAM VK_SNAPSHOT   = 0x2C;
	static constexpr WPARAM VK_F9         = 0x78;
	static constexpr WPARAM VK_F10        = 0x79;
	static constexpr WPARAM VK_F11        = 0x7A;
	static constexpr WPARAM VK_F12        = 0x7B;
	static constexpr WPARAM VK_PROCESSKEY = 0xE5;

	static constexpr UINT MAPVK_VK_TO_VSC    = 0;
	static constexpr UINT MAPVK_VSC_TO_VK    = 1;
	static constexpr UINT MAPVK_VK_TO_CHAR   = 2;
	static constexpr UINT MAPVK_VSC_TO_VK_EX = 3;
	static constexpr UINT MAPVK_VK_TO_VSC_EX = 4;

	static const HWND HWND_BOTTOM    = reinterpret_cast<HWND>(1);
	static const HWND HWND_NOTOPMOST = reinterpret_cast<HWND>(-2);
	static const HWND HWND_TOP       = reinterpret_cast<HWND>(0);
	static const HWND HWND_TOPMOST   = reinterpret_cast<HWND>(-1);

	static constexpr DWORD LWA_ALPHA    = 0x2;
	static constexpr DWORD LWA_COLORKEY = 0x1;

	static constexpr DWORD WHEEL_DELTA = 120;

	static constexpr DWORD RID_HEADER = 0x10000005;
	static constexpr DWORD RID_INPUT  = 0x10000003;

	static constexpr DWORD MOUSE_MOVE_RELATIVE      = 0x00;
	static constexpr DWORD MOUSE_MOVE_ABSOLUTE      = 0x01;
	static constexpr DWORD MOUSE_VIRTUAL_DESKTOP    = 0x02;
	static constexpr DWORD MOUSE_ATTRIBUTES_CHANGED = 0x04;
	static constexpr DWORD MOUSE_MOVE_NOCOALESCE    = 0x08;

	static constexpr DWORD TME_CANCEL    = 0x80000000;
	static constexpr DWORD TME_HOVER     = 0x00000001;
	static constexpr DWORD TME_LEAVE     = 0x00000002;
	static constexpr DWORD TME_NONCLIENT = 0x00000010;
	static constexpr DWORD TME_QUERY     = 0x40000000;

	static constexpr DWORD XBUTTON1 = 0x0001;
	static constexpr DWORD XBUTTON2 = 0x0002;

	static constexpr INT HTBORDER      = 18;
	static constexpr INT HTBOTTOM      = 15;
	static constexpr INT HTBOTTOMLEFT  = 16;
	static constexpr INT HTBOTTOMRIGHT = 17;
	static constexpr INT HTCAPTION     = 2;
	static constexpr INT HTCLIENT      = 1;
	static constexpr INT HTCLOSE       = 20;
	static constexpr INT HTERROR       = -2;
	static constexpr INT HTGROWBOX     = 4;
	static constexpr INT HTHELP        = 21;
	static constexpr INT HTHSCROLL     = 6;
	static constexpr INT HTLEFT        = 10;
	static constexpr INT HTMENU        = 5;
	static constexpr INT HTMAXBUTTON   = 9;
	static constexpr INT HTMINBUTTON   = 8;
	static constexpr INT HTNOWHERE     = 0;
	static constexpr INT HTREDUCE      = 8;
	static constexpr INT HTRIGHT       = 11;
	static constexpr INT HTSIZE        = 4;
	static constexpr INT HTSYSMENU     = 3;
	static constexpr INT HTTOP         = 12;
	static constexpr INT HTTOPLEFT     = 13;
	static constexpr INT HTTOPRIGHT    = 14;
	static constexpr INT HTTRANSPARENT = -1;
	static constexpr INT HTVSCROLL     = 7;
	static constexpr INT HTZOOM        = 9;

	static constexpr DWORD OWNER_SECURITY_INFORMATION               = 0x00000001L;
	static constexpr DWORD GROUP_SECURITY_INFORMATION               = 0x00000002L;
	static constexpr DWORD DACL_SECURITY_INFORMATION                = 0x00000004L;
	static constexpr DWORD SACL_SECURITY_INFORMATION                = 0x00000008L;
	static constexpr DWORD LABEL_SECURITY_INFORMATION               = 0x00000010L;
	static constexpr DWORD ATTRIBUTE_SECURITY_INFORMATION           = 0x00000020L;
	static constexpr DWORD SCOPE_SECURITY_INFORMATION               = 0x00000040L;
	static constexpr DWORD PROCESS_TRUST_LABEL_SECURITY_INFORMATION = 0x00000080L;
	static constexpr DWORD ACCESS_FILTER_SECURITY_INFORMATION       = 0x00000100L;
	static constexpr DWORD BACKUP_SECURITY_INFORMATION              = 0x00010000L;
	static constexpr DWORD PROTECTED_DACL_SECURITY_INFORMATION      = 0x80000000L;
	static constexpr DWORD PROTECTED_SACL_SECURITY_INFORMATION      = 0x40000000L;
	static constexpr DWORD UNPROTECTED_DACL_SECURITY_INFORMATION    = 0x20000000L;
	static constexpr DWORD UNPROTECTED_SACL_SECURITY_INFORMATION    = 0x10000000L;

	static constexpr DWORD WVR_ALIGNTOP    = 0x0010;
	static constexpr DWORD WVR_ALIGNRIGHT  = 0x0080;
	static constexpr DWORD WVR_ALIGNLEFT   = 0x0020;
	static constexpr DWORD WVR_ALIGNBOTTOM = 0x0040;
	static constexpr DWORD WVR_HREDRAW     = 0x0100;
	static constexpr DWORD WVR_VREDRAW     = 0x0200;
	static constexpr DWORD WVR_REDRAW      = 0x0300;
	static constexpr DWORD WVR_VALIDRECTS  = 0x0400;

	static constexpr INT SM_ARRANGE                     = 56;
	static constexpr INT SM_CLEANBOOT                   = 67;
	static constexpr INT SM_CMONITORS                   = 80;
	static constexpr INT SM_CMOUSEBUTTONS               = 43;
	static constexpr INT SM_CONVERTIBLESLATEMODE        = 0x2003;
	static constexpr INT SM_CXBORDER                    = 5;
	static constexpr INT SM_CXCURSOR                    = 13;
	static constexpr INT SM_CXDLGFRAME                  = 7;
	static constexpr INT SM_CXDOUBLECLK                 = 36;
	static constexpr INT SM_CXDRAG                      = 68;
	static constexpr INT SM_CXEDGE                      = 45;
	static constexpr INT SM_CXFIXEDFRAME                = 7;
	static constexpr INT SM_CXFOCUSBORDER               = 83;
	static constexpr INT SM_CXFRAME                     = 32;
	static constexpr INT SM_CXFULLSCREEN                = 16;
	static constexpr INT SM_CXHSCROLL                   = 21;
	static constexpr INT SM_CXHTHUMB                    = 10;
	static constexpr INT SM_CXICON                      = 11;
	static constexpr INT SM_CXICONSPACING               = 38;
	static constexpr INT SM_CXMAXIMIZED                 = 61;
	static constexpr INT SM_CXMAXTRACK                  = 59;
	static constexpr INT SM_CXMENUCHECK                 = 71;
	static constexpr INT SM_CXMENUSIZE                  = 54;
	static constexpr INT SM_CXMIN                       = 28;
	static constexpr INT SM_CXMINIMIZED                 = 57;
	static constexpr INT SM_CXMINSPACING                = 47;
	static constexpr INT SM_CXMINTRACK                  = 34;
	static constexpr INT SM_CXPADDEDBORDER              = 92;
	static constexpr INT SM_CXSCREEN                    = 0;
	static constexpr INT SM_CXSIZE                      = 30;
	static constexpr INT SM_CXSIZEFRAME                 = 32;
	static constexpr INT SM_CXSMICON                    = 49;
	static constexpr INT SM_CXSMSIZE                    = 52;
	static constexpr INT SM_CXVIRTUALSCREEN             = 78;
	static constexpr INT SM_CXVSCROLL                   = 2;
	static constexpr INT SM_CYBORDER                    = 6;
	static constexpr INT SM_CYCAPTION                   = 4;
	static constexpr INT SM_CYCURSOR                    = 14;
	static constexpr INT SM_CYDLGFRAME                  = 8;
	static constexpr INT SM_CYDOUBLECLK                 = 37;
	static constexpr INT SM_CYDRAG                      = 69;
	static constexpr INT SM_CYEDGE                      = 46;
	static constexpr INT SM_CYFIXEDFRAME                = 8;
	static constexpr INT SM_CYFOCUSBORDER               = 84;
	static constexpr INT SM_CYFRAME                     = 33;
	static constexpr INT SM_CYFULLSCREEN                = 17;
	static constexpr INT SM_CYHSCROLL                   = 3;
	static constexpr INT SM_CYICON                      = 12;
	static constexpr INT SM_CYICONSPACING               = 39;
	static constexpr INT SM_CYKANJIWINDOW               = 18;
	static constexpr INT SM_CYMAXIMIZED                 = 62;
	static constexpr INT SM_CYMAXTRACK                  = 60;
	static constexpr INT SM_CYMENU                      = 15;
	static constexpr INT SM_CYMENUCHECK                 = 72;
	static constexpr INT SM_CYMENUSIZE                  = 55;
	static constexpr INT SM_CYMIN                       = 29;
	static constexpr INT SM_CYMINIMIZED                 = 58;
	static constexpr INT SM_CYMINSPACING                = 48;
	static constexpr INT SM_CYMINTRACK                  = 35;
	static constexpr INT SM_CYSCREEN                    = 1;
	static constexpr INT SM_CYSIZE                      = 31;
	static constexpr INT SM_CYSIZEFRAME                 = 33;
	static constexpr INT SM_CYSMCAPTION                 = 51;
	static constexpr INT SM_CYSMICON                    = 50;
	static constexpr INT SM_CYSMSIZE                    = 53;
	static constexpr INT SM_CYVIRTUALSCREEN             = 79;
	static constexpr INT SM_CYVSCROLL                   = 20;
	static constexpr INT SM_CYVTHUMB                    = 9;
	static constexpr INT SM_DBCSENABLED                 = 42;
	static constexpr INT SM_DEBUG                       = 22;
	static constexpr INT SM_DIGITIZER                   = 94;
	static constexpr INT SM_IMMENABLED                  = 82;
	static constexpr INT SM_MAXIMUMTOUCHES              = 95;
	static constexpr INT SM_MEDIACENTER                 = 87;
	static constexpr INT SM_MENUDROPALIGNMENT           = 40;
	static constexpr INT SM_MIDEASTENABLED              = 74;
	static constexpr INT SM_MOUSEPRESENT                = 19;
	static constexpr INT SM_MOUSEHORIZONTALWHEELPRESENT = 91;
	static constexpr INT SM_MOUSEWHEELPRESENT           = 75;
	static constexpr INT SM_NETWORK                     = 63;
	static constexpr INT SM_PENWINDOWS                  = 41;
	static constexpr INT SM_REMOTECONTROL               = 0x2001;
	static constexpr INT SM_REMOTESESSION               = 0x1000;
	static constexpr INT SM_SAMEDISPLAYFORMAT           = 81;
	static constexpr INT SM_SECURE                      = 44;
	static constexpr INT SM_SERVERR2                    = 89;
	static constexpr INT SM_SHOWSOUNDS                  = 70;
	static constexpr INT SM_SHUTTINGDOWN                = 0x2000;
	static constexpr INT SM_SLOWMACHINE                 = 73;
	static constexpr INT SM_STARTER                     = 88;
	static constexpr INT SM_SWAPBUTTON                  = 23;
	static constexpr INT SM_SYSTEMDOCKED                = 0x2004;
	static constexpr INT SM_TABLETPC                    = 86;
	static constexpr INT SM_XVIRTUALSCREEN              = 76;
	static constexpr INT SM_YVIRTUALSCREEN              = 77;

	constexpr std::int32_t LOWORD(LPARAM l)
	{
		return static_cast<std::int16_t>(l & 0xFFFF);
	}

	constexpr std::int32_t HIWORD(LPARAM l)
	{
		return static_cast<std::int16_t>((l >> 16) & 0xFFFF);
	}

	constexpr std::uint32_t GET_XBUTTON_WPARAM(WPARAM wParam)
	{
		return HIWORD(wParam);
	}

	constexpr std::int32_t GET_X_LPARAM(LPARAM lp)
	{
		return ((int) (short) LOWORD(lp));
	}

	constexpr std::int32_t GET_Y_LPARAM(LPARAM lp)
	{
		return ((int) (short) HIWORD(lp));
	}

	constexpr DWORD MAKELANGID(DWORD p, DWORD s)
	{
		return s << 10 | p;
	}
} // namespace Brainfryer::Windows