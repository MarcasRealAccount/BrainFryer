#include "Brainfryer/Envs/Windows/Win32Window.h"
#include "Brainfryer/Input/Devices/Keyboard.h"
#include "Brainfryer/Input/Devices/Mouse.h"
#include "Brainfryer/Utils/Log.h"
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

	static std::vector<Monitor>      s_Monitors;
	static std::vector<Win32Window*> s_Windows;
	static std::function<void()>     s_MainOnRender;
	static Win32Window*              s_CurrentFocused;

	static std::uint32_t s_Keycodes[512] {
		0xffffffff, 0x100, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2d, 0x3d, 0x103, 0x102, 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4f, 0x50, 0x5b, 0x5d, 0x101, 0x155, 0x41, 0x53, 0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4c, 0x3b, 0x27, 0x60, 0x154, 0x5c, 0x5a, 0x58, 0x43, 0x56, 0x42, 0x4e, 0x4d, 0x2c, 0x2e, 0x2f, 0x158, 0x14c, 0x156, 0x20, 0x118, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127, 0x128, 0x129, 0x12a, 0x12b, 0x11c, 0x119, 0x147, 0x148, 0x149, 0x14d, 0x144, 0x145, 0x146, 0x14e, 0x141, 0x142, 0x143, 0x140, 0x14a, 0xffffffff, 0xffffffff, 0xa2, 0x12c, 0x12d, 0x150, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x12e, 0x12f, 0x130, 0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137, 0x138, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x139, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x14f, 0x159, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x14b, 0xffffffff, 0x11b, 0x15a, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x11a, 0xffffffff, 0x10c, 0x109, 0x10a, 0xffffffff, 0x107, 0xffffffff, 0x106, 0xffffffff, 0x10d, 0x108, 0x10b, 0x104, 0x105, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x157, 0x15b, 0x15c, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	};

	static std::uint32_t s_Scancodes[512] {
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x39, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x28, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x33, 0xc, 0x34, 0x35, 0xb, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xffffffff, 0x27, 0xffffffff, 0xd, 0xffffffff, 0xffffffff, 0xffffffff, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0xffffffff, 0xffffffff, 0x29, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x56, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x1, 0x1c, 0xf, 0xe, 0x152, 0x153, 0x14d, 0x14b, 0x150, 0x148, 0x149, 0x151, 0x147, 0x14f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x3a, 0x46, 0x145, 0x137, 0x45, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x57, 0x58, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x76, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x52, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47, 0x48, 0x49, 0x53, 0x135, 0x37, 0x4a, 0x4e, 0x11c, 0x59, 0xffffffff, 0xffffffff, 0xffffffff, 0x2a, 0x1d, 0x38, 0x15b, 0x36, 0x11d, 0x138, 0x15c, 0x15d, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	};

	static LPCWSTR Win32CursorMode(ECursor cursor);

	LRESULT Win32WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		Win32Window* window = reinterpret_cast<Win32Window*>(GetPropW(hWnd, L"BrainFryer"));
		if (!window)
			return DefWindowProcW(hWnd, Msg, wParam, lParam);

		++window->m_EventCount;

		LRESULT result = 0;
		if (window->e_WindowMsg(result, window, Msg, wParam, lParam))
			return result;

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
			case SC_KEYMENU:
				return 0;
			}
			break;
		case WM_SETCURSOR:
		{
			std::int32_t ht = LOWORD(lParam);
			switch (ht)
			{
			case HTCAPTION:
			case HTCLIENT:
			case HTCLOSE:
			case HTMAXBUTTON:
			case HTMINBUTTON:
			case HTHELP:
				window->setCursor(window->m_Cursor);
				return 1;
			case HTBOTTOM:
			case HTTOP:
				SetCursor(LoadCursorW(nullptr, Win32CursorMode(ECursor::SizeNS)));
				return 1;
			case HTLEFT:
			case HTRIGHT:
				SetCursor(LoadCursorW(nullptr, Win32CursorMode(ECursor::SizeWE)));
				return 1;
			case HTBOTTOMLEFT:
			case HTTOPRIGHT:
				SetCursor(LoadCursorW(nullptr, Win32CursorMode(ECursor::SizeNESW)));
				return 1;
			case HTBOTTOMRIGHT:
			case HTTOPLEFT:
				SetCursor(LoadCursorW(nullptr, Win32CursorMode(ECursor::SizeNWSE)));
				return 1;
			}
			return 0;
		}
		case WM_SETFOCUS:
			if (s_CurrentFocused)
			{
				s_CurrentFocused->m_Focused = false;
				s_CurrentFocused->e_Focus(s_CurrentFocused, false);
				s_CurrentFocused = nullptr;
			}
			s_CurrentFocused  = window;
			window->m_Focused = true;
			window->e_Focus(window, true);
			return 0;
		case WM_KILLFOCUS:
			if (s_CurrentFocused)
			{
				s_CurrentFocused->m_Focused = false;
				s_CurrentFocused->e_Focus(s_CurrentFocused, false);
				s_CurrentFocused = nullptr;
			}
			return 0;
		case WM_CLOSE:
			window->m_RequestedClose = true;
			return 0;
		case WM_ENTERSIZEMOVE:
			//SetTimer(hWnd, 1, 10, nullptr);
			break;
		case WM_EXITSIZEMOVE:
			//KillTimer(hWnd, 1);
			break;
		case WM_TIMER:
			if (wParam == 1)
			{
				if (s_MainOnRender)
				{
					s_MainOnRender();
					for (auto wnd : s_Windows)
						wnd->e_Render(wnd);
				}
			}
			break;
		case WM_SIZE:
		{
			if (window->m_Specs.state != EWindowState::Fullscreen)
			{
				auto origState = window->m_Specs.state;
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
				if (origState != window->m_Specs.state)
					window->e_State(window, window->m_Specs.state);
			}
			std::uint32_t origW = window->m_Specs.rect.w;
			std::uint32_t origH = window->m_Specs.rect.h;
			std::uint32_t newW  = LOWORD(lParam);
			std::uint32_t newH  = HIWORD(lParam);
			if (newW != origW || newH != origH)
			{
				window->m_Specs.rect.w = newW;
				window->m_Specs.rect.h = newH;
				window->e_Size(window, newW, newH);
			}
			return 0;
		}
		case WM_MOVE:
		{
			std::uint32_t origX = window->m_Specs.rect.x;
			std::uint32_t origY = window->m_Specs.rect.y;
			std::uint32_t newX  = LOWORD(lParam);
			std::uint32_t newY  = HIWORD(lParam);
			if (newX != origX || newY != origY)
			{
				window->m_Specs.rect.x = newX;
				window->m_Specs.rect.y = newY;
				window->e_Move(window, newX, newY);
			}
			return 0;
		}
		case WM_UNICHAR:
		{
			if (wParam == UNICODE_NOCHAR)
				return true;

			window->e_Char(window, static_cast<std::uint32_t>(wParam));
			return 0;
		}
		case WM_CHAR:
		case WM_SYSCHAR:
		{
			if (wParam >= 0xD800 && wParam <= 0xDBFF)
			{
				window->m_HighSurrogate = static_cast<std::uint16_t>(wParam);
			}
			else
			{
				std::uint32_t codepoint = 0;
				if (wParam >= 0xDC00 && wParam <= 0xDFFF)
				{
					if (window->m_HighSurrogate)
					{
						codepoint += static_cast<std::uint32_t>(window->m_HighSurrogate - 0xD800) << 10;
						codepoint += static_cast<std::uint32_t>(wParam) - 0xDC00;
						codepoint += 0x10000;
					}
				}
				else
					codepoint = static_cast<std::uint32_t>(wParam);

				window->m_HighSurrogate = 0;
				window->e_Char(window, codepoint);
			}
			return 0;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			std::uint32_t       key, scancode;
			auto                keyState = HIWORD(lParam);
			Input::EButtonState state    = keyState & KF_UP     ? Input::EButtonState::Released :
			                               keyState & KF_REPEAT ? Input::EButtonState::Repeated :
			                                                      Input::EButtonState::Pressed;

			scancode = HIWORD(lParam) & (KF_EXTENDED | 0xFF);
			if (!scancode)
				scancode = MapVirtualKeyW(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);

			switch (scancode)
			{
			case 0x54:
				scancode = 0x137;
				break;
			case 0x146:
				scancode = 0x45;
				break;
			case 0x136:
				scancode = 0x36;
				break;
			}

			key = s_Keycodes[scancode];
			if (key == ~0U)
				break;

			if (wParam == VK_CONTROL)
			{
				if (HIWORD(lParam) & KF_EXTENDED)
				{
					key = Input::ButtonIndices::KeyRightControl;
				}
				else
				{
					MSG         next;
					const DWORD time = GetMessageTime();

					if (PeekMessageW(&next, nullptr, 0, 0, PM_NOREMOVE))
						if (next.message == WM_KEYDOWN || next.message == WM_SYSKEYDOWN || next.message == WM_KEYUP || next.message == WM_SYSKEYUP)
							if (next.wParam == VK_MENU && (HIWORD(next.lParam) & KF_EXTENDED) && next.time == time)
								break;

					key = Input::ButtonIndices::KeyLeftControl;
				}
			}
			else if (wParam == VK_PROCESSKEY)
			{
				break;
			}

			if (state == Input::EButtonState::Released && wParam == VK_SHIFT)
			{
				window->e_Key(window, Input::ButtonIndices::KeyLeftShift, scancode, state);
				window->e_Key(window, Input::ButtonIndices::KeyRightShift, scancode, state);
			}
			else if (wParam == VK_SNAPSHOT)
			{
				window->e_Key(window, key, scancode, Input::EButtonState::Pressed);
				window->e_Key(window, key, scancode, Input::EButtonState::Released);
			}
			else
				window->e_Key(window, key, scancode, state);

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			std::uint32_t       button;
			Input::EButtonState state;

			if (Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP)
				button = Input::ButtonIndices::MouseLeft;
			else if (Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP)
				button = Input::ButtonIndices::MouseRight;
			else if (Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP)
				button = Input::ButtonIndices::MouseMiddle;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = Input::ButtonIndices::Mouse4;
			else
				button = Input::ButtonIndices::Mouse5;

			if (Msg == WM_LBUTTONDOWN || Msg == WM_RBUTTONDOWN ||
			    Msg == WM_MBUTTONDOWN || Msg == WM_XBUTTONDOWN)
				state = Input::EButtonState::Pressed;
			else
				state = Input::EButtonState::Released;

			window->e_MouseButton(window, button, state);

			if (Msg == WM_XBUTTONDOWN || Msg == WM_XBUTTONUP)
				return 1;

			return 0;
		}
		case WM_MOUSEMOVE:
		{
			std::int32_t x = GET_X_LPARAM(lParam);
			std::int32_t y = GET_Y_LPARAM(lParam);

			if (!window->m_CursorTracked)
			{
				TRACKMOUSEEVENT tme {};
				tme.cbSize    = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags   = TME_LEAVE;
				tme.hwndTrack = window->m_HWnd;
				TrackMouseEvent(&tme);

				window->m_CursorTracked = true;
				window->e_MouseEnterExit(window, true);
			}

			if (window->m_CursorMode == ECursorMode::Disabled)
			{
				std::int32_t dx = x - window->m_LastCursorPos.x;
				std::int32_t dy = y - window->m_LastCursorPos.y;

				if (dx != 0 || dy != 0)
				{
					window->m_VirtualCursorPos.x += dx;
					window->m_VirtualCursorPos.y += dy;
					window->e_MouseMove(window, static_cast<float>(window->m_VirtualCursorPos.x), static_cast<float>(window->m_VirtualCursorPos.y));
				}
			}
			else
			{
				if (x != window->m_LastCursorPos.x || y != window->m_LastCursorPos.y)
					window->e_MouseMove(window, static_cast<float>(x), static_cast<float>(y));
			}
			window->m_LastCursorPos = { x, y };
			return 0;
		}
		case WM_INPUT:
		{
			if (window->m_CursorMode != ECursorMode::Raw)
				break;

			RAWINPUT* ri = reinterpret_cast<RAWINPUT*>(lParam);

			UINT size;
			GetRawInputData(ri, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

			if (size >= window->m_RawInput.size())
				window->m_RawInput.resize(size);

			GetRawInputData(ri, RID_INPUT, window->m_RawInput.data(), &size, sizeof(RAWINPUTHEADER));

			RAWINPUT*    data = reinterpret_cast<RAWINPUT*>(window->m_RawInput.data());
			std::int32_t dx   = 0;
			std::int32_t dy   = 0;
			if (data->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
			{
				dx = data->data.mouse.lLastX - window->m_LastCursorPos.x;
				dy = data->data.mouse.lLastY - window->m_LastCursorPos.y;
			}
			else
			{
				dx = data->data.mouse.lLastX;
				dy = data->data.mouse.lLastY;
			}

			window->m_VirtualCursorPos.x += dx;
			window->m_VirtualCursorPos.y += dy;
			window->e_MouseMove(window, static_cast<float>(window->m_VirtualCursorPos.x), static_cast<float>(window->m_VirtualCursorPos.y));
			window->m_LastCursorPos.x += dx;
			window->m_LastCursorPos.y += dy;
			break;
		}
		case WM_MOUSELEAVE:
			window->m_CursorTracked = false;
			window->e_MouseEnterExit(window, false);
			return 0;
		case WM_MOUSEWHEEL:
			window->e_MouseScroll(window, 0.0f, HIWORD(wParam) / static_cast<float>(WHEEL_DELTA));
			return 0;
		case WM_MOUSEHWHEEL:
			window->e_MouseScroll(window, -HIWORD(wParam) / static_cast<float>(WHEEL_DELTA), 0.0f);
			return 0;
		case WM_DPICHANGED:
		{
			float xScale       = HIWORD(wParam) / 96.0f;
			window->m_DPIScale = xScale;
			window->e_DPIScale(window, xScale);
			break;
		}
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	static HINSTANCE GetInstance()
	{
		return static_cast<HINSTANCE>(GetModuleHandleW(nullptr));
	}

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

	LPCWSTR Win32CursorMode(ECursor cursor)
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

	bool Win32Window::GetKeyState(std::uint32_t keycode)
	{
		auto scancode = s_Scancodes[keycode];
		auto vk       = MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX);
		return Brainfryer::Windows::GetKeyState(vk) & 0x8000;
	}

	Point Win32Window::GetCursorPos()
	{
		POINT pos;
		Brainfryer::Windows::GetCursorPos(&pos);
		return { pos.x, pos.y };
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
		for (auto window : s_Windows)
		{
			Log::Info("{} window events per frame", window->m_EventCount);
			window->m_EventCount = 0;
		}

		MSG msg {};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Win32Window::SetMainOnRender(std::function<void()> mainOnRender)
	{
		s_MainOnRender = std::move(mainOnRender);
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

	static void Win32WindowFlags(EWindowFlags flags, DWORD& dwStyle, DWORD& dwExStyle)
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
	      m_DPIScale(1.0f),
	      m_Cursor(ECursor::Arrow),
	      m_CursorMode(ECursorMode::Normal)
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
			Win32Window::fullscreen();
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

	void Win32Window::setCursorMode(ECursorMode mode)
	{
		// TODO(MarcasRealAccount): Handle cursor mode change
		m_CursorMode = mode;
	}

	void Win32Window::setCursor(ECursor cursor)
	{
		SetCursor(LoadCursorW(nullptr, Win32CursorMode(cursor)));
		m_Cursor = cursor;
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