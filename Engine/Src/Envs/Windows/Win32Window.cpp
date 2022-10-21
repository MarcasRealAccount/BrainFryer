#include "Brainfryer/Envs/Windows/Win32Window.h"
#include "Brainfryer/Utils/UTFConv.h"

namespace Brainfryer::Windows
{
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

	HINSTANCE GetInstance()
	{
		return static_cast<HINSTANCE>(GetModuleHandleW(nullptr));
	}

	static struct Win32ClassRegister
	{
	public:
		Win32ClassRegister()
		    : m_WndClassAtom(0)
		{
			WNDCLASSEXW wndClass;
			wndClass.cbSize        = sizeof(WNDCLASSEXW);
			wndClass.style         = CS_VREDRAW | CS_HREDRAW;
			wndClass.lpfnWndProc   = &Win32WinProc;
			wndClass.cbClsExtra    = 0;
			wndClass.cbWndExtra    = 0;
			wndClass.hInstance     = GetInstance();
			wndClass.hIcon         = nullptr;
			wndClass.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
			wndClass.hbrBackground = nullptr;
			wndClass.lpszMenuName  = nullptr;
			wndClass.lpszClassName = L"BrainFryerClass";
			wndClass.hIconSm       = nullptr;

			m_WndClassAtom = RegisterClassExW(&wndClass);
			if (!m_WndClassAtom)
				Win32Window::HandleLastError("RegisterClassExW");
		}

		~Win32ClassRegister()
		{
			if (!m_WndClassAtom)
				return;

			if (!UnregisterClassW(L"BrainFryerClass", GetInstance()))
				Win32Window::HandleLastError("UnregisterClassExW");
		}

		bool isRegistered() const { return !!m_WndClassAtom; }

		ATOM m_WndClassAtom;
	} s_ClassRegister;

	void Win32Window::FatalErrorBox(std::string_view message)
	{
		std::wstring msg = Utils::UTF::ConvertUTF8ToWide(message);
		MessageBoxW(nullptr, msg.c_str(), L"Brain was completely fried", MB_OK);
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

	Win32Window::Win32Window(WindowSpecification specs)
	    : m_Specs(std::move(specs)),
	      m_PState(m_Specs.state),
	      m_PPState(EWindowState::Normal),
	      m_HWnd(nullptr),
	      m_PPlacement({ sizeof(WINDOWPLACEMENT) }),
	      m_RequestedClose(false)
	{
		if (!s_ClassRegister.isRegistered())
			return;

		std::int32_t x = m_Specs.rect.x;
		std::int32_t y = m_Specs.rect.y;
		if (x == 1 << 31)
			x = (GetSystemMetrics(SM_CXSCREEN) - m_Specs.rect.w) >> 1;
		if (y == 1 << 31)
			y = (GetSystemMetrics(SM_CYSCREEN) - m_Specs.rect.h) >> 1;

		std::wstring title = Utils::UTF::ConvertUTF8ToWide(m_Specs.title);
		m_HWnd             = CreateWindowExW(
            0,
            L"BrainFryerClass",
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
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
	}

	Win32Window::~Win32Window()
	{
		if (m_HWnd)
			DestroyWindow(m_HWnd);
		m_HWnd = nullptr;
	}

	void Win32Window::msgLoop()
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

	void Win32Window::setTitle(std::string title)
	{
		m_Specs.title = std::move(title);

		if (m_HWnd)
		{
			std::wstring titleU16 = Utils::UTF::ConvertUTF8ToWide(m_Specs.title);
			SetWindowTextW(m_HWnd, titleU16.c_str());
		}
	}

	void Win32Window::setWindowRect(Rect rect)
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

	void Win32Window::show()
	{
		m_Specs.visible = true;
		if (m_HWnd)
		{
			switch (m_PPState)
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
			case EWindowState::Fullscreen:
				ShowWindow(m_HWnd, SW_NORMAL);
				break;
			}
		}
	}

	void Win32Window::requestClose(bool request)
	{
		m_RequestedClose = request;
	}

	std::string_view Win32Window::title() const
	{
		return m_Specs.title;
	}

	Rect Win32Window::windowRect() const
	{
		return m_Specs.rect;
	}

	EWindowState Win32Window::state() const
	{
		return m_Specs.state;
	}

	bool Win32Window::visible() const
	{
		return m_Specs.visible;
	}

	bool Win32Window::requestedClose() const
	{
		return m_RequestedClose;
	}
} // namespace Brainfryer::Windows