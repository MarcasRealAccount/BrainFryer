#include "Brainfryer/Envs/Windows/Win32Window.h"
#include "Brainfryer/Utils/UTFConv.h"

#include <spdlog/fmt/fmt.h>

#include <algorithm>

namespace Brainfryer::Windows
{
	enum class MONITOR_DPI_TYPE
	{
		EffectiveDPI = 0,
		AngularDPI   = 1,
		RawDPI       = 2,
		Default      = EffectiveDPI
	};
	using HRESULT              = std::int32_t;
	using PFN_GetDpiForMonitor = HRESULT (*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

	LRESULT Win32WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		Win32Window* window = reinterpret_cast<Win32Window*>(GetPropW(hWnd, L"BrainFryer"));

		switch (Msg)
		{
		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				if (window->state() == EWindowState::Fullscreen)
					return 0;
				break;
			}
			break;
		case WM_CLOSE:
			window->m_RequestedClose = true;
			return 0;
		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_SIZE:
			if (window->m_Specs.state != EWindowState::Fullscreen)
			{
				switch (wParam)
				{
				case SIZE_RESTORED:
					window->m_Specs.state = EWindowState::Normal;
					break;
				case SIZE_MINIMIZED:
					window->m_Specs.state = EWindowState::Iconified;
					break;
				case SIZE_MAXIMIZED:
					window->m_Specs.state = EWindowState::Maximized;
					break;
				}
			}
			window->m_Specs.rect.w = LOWORD(lParam);
			window->m_Specs.rect.h = HIWORD(lParam);
			return 0;
		case WM_MOVE:
			window->m_Specs.rect.x = LOWORD(lParam);
			window->m_Specs.rect.y = HIWORD(lParam);
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_F12)
				if (window->m_Specs.visible)
					window->hide();
				else
					window->show();
			else if (wParam == VK_F11)
				window->fullscreen(window->m_Specs.state != EWindowState::Fullscreen);
			else if (wParam == VK_F10)
				if (window->m_Specs.state != EWindowState::Maximized)
					window->maximize();
				else
					window->restore();
			else if (wParam == VK_F9)
				if (window->m_Specs.state != EWindowState::Iconified)
					window->iconify();
				else
					window->restore();
			return 0;
		}
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	static HINSTANCE GetInstance()
	{
		return static_cast<HINSTANCE>(GetModuleHandleW(nullptr));
	}

	static std::vector<Monitor>      s_Monitors;
	static std::vector<Win32Window*> s_Windows;

	static float GetDPIScaleForMonitor(HMONITOR hMonitor);

	static BOOL EnumMonitorsProc(HMONITOR hMonitor, [[maybe_unused]] HDC hDC, [[maybe_unused]] RECT* rect, [[maybe_unused]] LPARAM lParam)
	{
		auto& monitor = s_Monitors.emplace_back();

		MONITORINFOEXW info {};
		info.cbSize = sizeof(MONITORINFOEXW);
		if (!GetMonitorInfoW(hMonitor, &info))
			return true;
		monitor.dpiScale = GetDPIScaleForMonitor(hMonitor);
		monitor.mainArea = { info.rcMonitor.left, info.rcMonitor.top, static_cast<std::uint32_t>(info.rcMonitor.right - info.rcMonitor.left), static_cast<std::uint32_t>(info.rcMonitor.bottom - info.rcMonitor.top) };
		monitor.workArea = { info.rcWork.left, info.rcWork.top, static_cast<std::uint32_t>(info.rcWork.right - info.rcWork.left), static_cast<std::uint32_t>(info.rcWork.bottom - info.rcWork.top) };
		return true;
	}

	static void EnumMonitors()
	{
		s_Monitors.clear();
		EnumDisplayMonitors(nullptr, nullptr, &EnumMonitorsProc, 0);
	}

	static struct Win32ClassRegister
	{
	public:
		Win32ClassRegister()
		    : m_WndClassAtom(0),
		      m_SHCoreDll(nullptr),
		      m_GetDPIForMonitor(nullptr)
		{
			if constexpr (Core::s_IsSystemWindows)
			{
				WNDCLASSEXW wndClass;
				wndClass.cbSize        = sizeof(WNDCLASSEXW);
				wndClass.style         = CS_VREDRAW | CS_HREDRAW;
				wndClass.lpfnWndProc   = &Win32WinProc;
				wndClass.cbClsExtra    = 0;
				wndClass.cbWndExtra    = 0;
				wndClass.hInstance     = GetInstance();
				wndClass.hIcon         = reinterpret_cast<HICON>(LoadCursorW(nullptr, IDC_ARROW));
				wndClass.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
				wndClass.hbrBackground = nullptr;
				wndClass.lpszMenuName  = nullptr;
				wndClass.lpszClassName = L"BrainFryerClass";
				wndClass.hIconSm       = nullptr;

				m_WndClassAtom = RegisterClassExW(&wndClass);
				if (!m_WndClassAtom)
					Win32Window::HandleLastError("RegisterClassExW");

				m_SHCoreDll = LoadLibraryW(L"shcore.dll");
				if (m_SHCoreDll)
				{
					m_GetDPIForMonitor = reinterpret_cast<PFN_GetDpiForMonitor>(GetProcAddress(m_SHCoreDll, "GetDpiForMonitor"));
					if (!m_GetDPIForMonitor)
					{
						if (!FreeLibrary(m_SHCoreDll))
							Win32Window::HandleLastError("FreeLibrary");
						m_SHCoreDll = nullptr;
					}
				}
				else
				{
					Win32Window::HandleLastError("LoadLibraryW");
				}

				EnumMonitors();
			}
		}

		~Win32ClassRegister()
		{
			if constexpr (Core::s_IsSystemWindows)
			{
				if (m_SHCoreDll && !FreeLibrary(m_SHCoreDll))
					Win32Window::HandleLastError("FreeLibrary");

				if (m_WndClassAtom && !UnregisterClassW(L"BrainFryerClass", GetInstance()))
					Win32Window::HandleLastError("UnregisterClassExW");
			}
		}

		bool isRegistered() const { return !!m_WndClassAtom; }

		ATOM                 m_WndClassAtom;
		HINSTANCE            m_SHCoreDll;
		PFN_GetDpiForMonitor m_GetDPIForMonitor;
	} s_ClassRegister;

	float GetDPIScaleForMonitor(HMONITOR hMonitor)
	{
		if (!s_ClassRegister.m_SHCoreDll)
			return 1.0f;

		UINT xdpi, ydpi;
		s_ClassRegister.m_GetDPIForMonitor(hMonitor, MONITOR_DPI_TYPE::EffectiveDPI, &xdpi, &ydpi);

		return xdpi / 96.0f;
	}

	static LPCWSTR Win32CursorMode(ECursor cursor)
	{
		switch (cursor)
		{
		case ECursor::Hidden: return nullptr;
		case ECursor::Arrow: return IDC_ARROW;
		case ECursor::IBeam: return IDC_IBEAM;
		case ECursor::SizeAll: return IDC_SIZEALL;
		case ECursor::SizeWE: return IDC_SIZEWE;
		case ECursor::SizeNS: return IDC_SIZENS;
		case ECursor::SizeNESW: return IDC_SIZENESW;
		case ECursor::SizeNWSE: return IDC_SIZENWSE;
		case ECursor::Hand: return IDC_HAND;
		case ECursor::No: return IDC_NO;
		}
		return IDC_ARROW;
	}

	const std::vector<Monitor>& Win32Window::GetMonitors()
	{
		return s_Monitors;
	}

	Point Win32Window::GetCursorPos()
	{
		POINT pos;
		Brainfryer::Windows::GetCursorPos(&pos);
		return { pos.x, pos.y };
	}

	void Win32Window::SetCursor(ECursor cursor)
	{
		Brainfryer::Windows::SetCursor(LoadCursorW(nullptr, Win32CursorMode(cursor)));
	}

	Window* Win32Window::GetFocusedWindow()
	{
		for (auto window : s_Windows)
			if (window->focused())
				return window;
		return nullptr;
	}

	Window* Win32Window::WindowFromPoint(Point pos)
	{
		HWND hWnd = Brainfryer::Windows::WindowFromPoint(POINT { pos.x, pos.y });
		for (auto window : s_Windows)
			if (window->handle() == hWnd)
				return window;
		return nullptr;
	}

	void Win32Window::MsgLoop()
	{
		MSG msg {};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Win32Window::FatalErrorBox(std::string_view message, std::string_view title, const Utils::BackTrace& backTrace)
	{
		std::wstring msg     = backTrace.frames().empty() ? Utils::UTF::ConvertUTF8ToWide(message) : Utils::UTF::ConvertUTF8ToWide(fmt::format("{}\n\n{}", message, backTrace));
		std::wstring caption = L"Brain was completely fried";
		if (!title.empty())
		{
			caption += L" by ";
			std::size_t start = caption.size();
			caption.resize(caption.size() + Utils::UTF::UTF8ToUTF16RequiredLength(title.data(), title.size()));
			std::size_t offset = 0;
			Utils::UTF::ConvertUTF8ToUTF16(title.data(), title.size(), caption.data() + start, caption.size() - start, offset);
		}
		MessageBoxW(nullptr, msg.c_str(), caption.c_str(), MB_OK);
	}

	void Win32Window::HandleLastError(std::string_view functionName)
	{
		DWORD ec = GetLastError();

		LPWSTR msg;
		FormatMessageW(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		    nullptr,
		    ec,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    reinterpret_cast<LPWSTR>(&msg),
		    0,
		    nullptr);

		std::size_t  textCapacity = lstrlenW(msg) + Utils::UTF::UTF8ToUTF16RequiredLength(functionName.data(), functionName.size()) + 40;
		std::wstring text(textCapacity, '\0');
		std::size_t  utf8Offset  = 0;
		std::size_t  utf16Offset = Utils::UTF::ConvertUTF8ToUTF16(functionName.data(), functionName.size(), text.data(), textCapacity, utf8Offset);
		std::swprintf(text.data() + utf16Offset, text.size() - utf16Offset, L" failed with error 0x%08X: %s", ec, msg);
		MessageBoxW(nullptr, text.c_str(), L"Brain was completely fried", MB_OK);

		LocalFree(msg);
	}

	void Win32WindowFlags(EWindowFlags flags, DWORD& dwStyle, DWORD& dwExStyle)
	{
		dwStyle   = 0;
		dwExStyle = 0;
		if (flags & WindowFlags::NoDecoration)
			dwStyle |= WS_POPUP;
		else
			dwStyle |= WS_OVERLAPPEDWINDOW;

		if (flags & WindowFlags::NoTaskBarIcon)
			dwExStyle |= WS_EX_TOOLWINDOW;
		else
			dwExStyle |= WS_EX_APPWINDOW;

		if (flags & WindowFlags::TopMost)
			dwExStyle |= WS_EX_TOPMOST;
	}

	Win32Window::Win32Window(WindowSpecification specs)
	    : m_Specs(std::move(specs)),
	      m_PState(m_Specs.state),
	      m_PPState(EWindowState::Normal),
	      m_HWnd(nullptr),
	      m_PPlacement({ sizeof(WINDOWPLACEMENT) }),
	      m_RequestedClose(false),
	      m_Focused(false),
	      m_DPIScale(1.0f)
	{
		if (!s_ClassRegister.isRegistered())
			return;

		std::int32_t x = m_Specs.rect.x;
		std::int32_t y = m_Specs.rect.y;
		if (x == 1 << 31)
			x = (GetSystemMetrics(SM_CXSCREEN) - m_Specs.rect.w) >> 1;
		if (y == 1 << 31)
			y = (GetSystemMetrics(SM_CYSCREEN) - m_Specs.rect.h) >> 1;

		DWORD dwStyle   = 0;
		DWORD dwExStyle = 0;
		Win32WindowFlags(m_Specs.flags, dwStyle, dwExStyle);

		std::wstring title = Utils::UTF::ConvertUTF8ToWide(m_Specs.title);
		m_HWnd             = CreateWindowExW(
            dwExStyle,
            L"BrainFryerClass",
            title.c_str(),
            dwStyle,
            x,
            y,
            m_Specs.rect.w,
            m_Specs.rect.h,
            nullptr,
            nullptr,
            GetInstance(),
            nullptr);
		if (!m_HWnd)
		{
			HandleLastError("CreateWindowExW");
			return;
		}

		SetPropW(m_HWnd, L"BrainFryer", this);

		switch (m_Specs.state)
		{
		case EWindowState::Normal:
			ShowWindow(m_HWnd, m_Specs.visible ? SW_NORMAL : SW_HIDE);
			break;
		case EWindowState::Iconified:
			ShowWindow(m_HWnd, m_Specs.visible ? SW_MINIMIZE : SW_HIDE);
			break;
		case EWindowState::Maximized:
			ShowWindow(m_HWnd, m_Specs.visible ? SW_MAXIMIZE : SW_HIDE);
			break;
		case EWindowState::Fullscreen:
			ShowWindow(m_HWnd, m_Specs.visible ? SW_NORMAL : SW_HIDE);
			fullscreen();
			break;
		}

		HMONITOR hMonitor = MonitorFromWindow(m_HWnd, MONITOR_DEFAULTTONEAREST);
		m_DPIScale        = GetDPIScaleForMonitor(hMonitor);

		s_Windows.emplace_back(this);
	}

	Win32Window::~Win32Window()
	{
		if (m_HWnd)
			DestroyWindow(m_HWnd);
		m_HWnd = nullptr;

		std::erase(s_Windows, this);
	}

	void Win32Window::setTitle(std::string title)
	{
		m_Specs.title = std::move(title);

		if (m_HWnd)
		{
			std::wstring titleU16 = Utils::UTF::ConvertUTF8ToWide(m_Specs.title);
			SetWindowTextW(m_HWnd, titleU16.c_str());
		}
	}

	void Win32Window::setFlags(EWindowFlags flags)
	{
		if (!m_HWnd || flags == m_Specs.flags)
			return;

		DWORD dwStyle   = 0;
		DWORD dwExStyle = 0;
		Win32WindowFlags(flags, dwStyle, dwExStyle);

		bool topMostChanged = (m_Specs.flags & WindowFlags::TopMost) != (flags & WindowFlags::TopMost);
		HWND insertAfter    = topMostChanged ? (flags & WindowFlags::TopMost ? HWND_TOPMOST : HWND_NOTOPMOST) : 0;
		UINT swpFlag        = topMostChanged ? 0 : SWP_NOZORDER;

		SetWindowLongW(m_HWnd, GWL_STYLE, dwStyle);
		SetWindowLongW(m_HWnd, GWL_EXSTYLE, dwExStyle);
		SetWindowPos(m_HWnd, insertAfter, 0, 0, 0, 0, swpFlag | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		ShowWindow(m_HWnd, SW_SHOWNA);
	}

	void Win32Window::setPos(Point pos)
	{
		m_Specs.rect.x = pos.x;
		m_Specs.rect.y = pos.y;
		if (m_HWnd)
		{
			std::int32_t x = m_Specs.rect.x;
			std::int32_t y = m_Specs.rect.y;
			if (x == 1 << 31)
				x = (GetSystemMetrics(SM_CXSCREEN) - m_Specs.rect.w) >> 1;
			if (y == 1 << 31)
				y = (GetSystemMetrics(SM_CYSCREEN) - m_Specs.rect.h) >> 1;

			SetWindowPos(m_HWnd,
			             nullptr,
			             x, y,
			             0, 0,
			             SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);
		}
	}

	void Win32Window::setSize(Size size)
	{
		m_Specs.rect.w = size.w;
		m_Specs.rect.h = size.h;
		if (m_HWnd)
		{
			SetWindowPos(m_HWnd,
			             nullptr,
			             0, 0,
			             size.w, size.h,
			             SWP_NOMOVE | SWP_NOZORDER | SWP_NOSENDCHANGING);
		}
	}

	void Win32Window::setRect(Rect rect)
	{
		m_Specs.rect = rect;
		if (m_HWnd)
		{
			std::int32_t x = m_Specs.rect.x;
			std::int32_t y = m_Specs.rect.y;
			if (x == 1 << 31)
				x = (GetSystemMetrics(SM_CXSCREEN) - m_Specs.rect.w) >> 1;
			if (y == 1 << 31)
				y = (GetSystemMetrics(SM_CYSCREEN) - m_Specs.rect.h) >> 1;

			SetWindowPos(m_HWnd,
			             nullptr,
			             x, y,
			             rect.w, rect.h,
			             SWP_NOZORDER | SWP_NOSENDCHANGING);
		}
	}

	void Win32Window::restore()
	{
		if (m_Specs.state == EWindowState::Fullscreen)
		{
			m_PState = EWindowState::Normal;
		}
		else if (m_Specs.state == EWindowState::Iconified && m_PPState == EWindowState::Fullscreen)
		{
			m_Specs.state = EWindowState::Fullscreen;
			m_PPState     = EWindowState::Normal;
			if (m_HWnd)
				ShowWindow(m_HWnd, SW_NORMAL);
		}
		else
		{
			m_Specs.state = EWindowState::Normal;
			if (m_HWnd)
				ShowWindow(m_HWnd, SW_NORMAL);
		}
	}

	void Win32Window::iconify()
	{
		if (m_HWnd)
		{
			if (m_Specs.state == EWindowState::Fullscreen)
				m_PPState = EWindowState::Fullscreen;
			else
				m_Specs.state = EWindowState::Iconified;

			ShowWindow(m_HWnd, SW_MINIMIZE);
		}
		else
		{
			m_Specs.state = EWindowState::Iconified;
		}
	}

	void Win32Window::maximize()
	{
		if (m_HWnd)
		{
			if (m_Specs.state == EWindowState::Fullscreen)
				m_PState = EWindowState::Maximized;
			else
				m_Specs.state = EWindowState::Maximized;
			ShowWindow(m_HWnd, SW_MAXIMIZE);
		}
		else
		{
			m_Specs.state = EWindowState::Maximized;
		}
	}

	void Win32Window::fullscreen(bool fullscreen)
	{
		m_PState      = fullscreen ? m_Specs.state : m_PState;
		m_Specs.state = EWindowState::Fullscreen;
		if (m_HWnd)
		{
			if (fullscreen)
			{
				MONITORINFOEXW mi;
				mi.cbSize = sizeof(MONITORINFOEXW);
				GetWindowPlacement(m_HWnd, &m_PPlacement);
				GetMonitorInfoW(MonitorFromWindow(m_HWnd, MONITOR_DEFAULTTOPRIMARY), &mi);

				SetWindowLongW(m_HWnd, GWL_STYLE, GetWindowLongW(m_HWnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(m_HWnd, nullptr,
				             mi.rcMonitor.left, mi.rcMonitor.top,
				             mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
				             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongW(m_HWnd, GWL_STYLE, GetWindowLongW(m_HWnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
				SetWindowPlacement(m_HWnd, &m_PPlacement);
				SetWindowPos(m_HWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

				m_Specs.state = m_PState;

				if (m_Specs.visible)
				{
					switch (m_Specs.state)
					{
					case EWindowState::Normal:
						ShowWindow(m_HWnd, SW_NORMAL);
						break;
					case EWindowState::Iconified:
						ShowWindow(m_HWnd, SW_MINIMIZE);
						break;
					case EWindowState::Maximized:
						ShowWindow(m_HWnd, SW_MAXIMIZE);
						break;
					}
				}
			}
		}
	}

	void Win32Window::hide()
	{
		m_Specs.visible = false;
		m_PPState       = m_Specs.state;
		if (m_HWnd)
			ShowWindow(m_HWnd, SW_HIDE);
	}

	void Win32Window::show(bool activate)
	{
		m_Specs.visible = true;
		if (m_HWnd)
		{
			switch (m_PPState)
			{
			case EWindowState::Normal:
				ShowWindow(m_HWnd, activate ? SW_NORMAL : SW_SHOWNOACTIVATE);
				break;
			case EWindowState::Iconified:
				ShowWindow(m_HWnd, activate ? SW_MINIMIZE : SW_MINIMIZENOACTIVATE);
				break;
			case EWindowState::Maximized:
				ShowWindow(m_HWnd, SW_MAXIMIZE);
				break;
			case EWindowState::Fullscreen:
				ShowWindow(m_HWnd, activate ? SW_NORMAL : SW_SHOWNOACTIVATE);
				break;
			}
		}
	}

	void Win32Window::focus()
	{
		if (!m_HWnd)
			return;

		BringWindowToTop(m_HWnd);
		SetForegroundWindow(m_HWnd);
		SetFocus(m_HWnd);
	}

	void Win32Window::requestClose(bool request)
	{
		m_RequestedClose = request;
	}

	void Win32Window::setAlpha(float alpha)
	{
		if (!(m_Specs.flags & WindowFlags::AlphaSupport))
			return;

		if (alpha < 1.0f)
		{
			DWORD style = GetWindowLongW(m_HWnd, GWL_EXSTYLE) | WS_EX_LAYERED;
			SetWindowLongW(m_HWnd, GWL_EXSTYLE, style);
			SetLayeredWindowAttributes(m_HWnd, 0, static_cast<BYTE>(255 * alpha), LWA_ALPHA);
		}
		else
		{
			DWORD style = GetWindowLongW(m_HWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED;
			SetWindowLongW(m_HWnd, GWL_EXSTYLE, style);
		}
	}

	Point Win32Window::screenToClient(Point pos) const
	{
		POINT point { pos.x, pos.y };
		ScreenToClient(m_HWnd, &point);
		return { point.x, point.y };
	}

	void Win32Window::setCursorPos(Point pos)
	{
		POINT point { pos.x, pos.y };
		ClientToScreen(m_HWnd, &point);
		SetCursorPos(point.x, point.y);
	}
} // namespace Brainfryer::Windows