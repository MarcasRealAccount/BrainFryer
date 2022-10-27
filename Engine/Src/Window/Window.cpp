#include "Brainfryer/Window/Window.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"

namespace Brainfryer
{
	const std::vector<Monitor>& Window::GetMonitors()
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetMonitors();
		else
			return {};
	}

	Point Window::GetCursorPos()
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetCursorPos();
		else
			return { 0, 0 };
	}

	void Window::SetCursor(ECursor cursor)
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::SetCursor(cursor);
	}

	Window* Window::GetFocusedWindow()
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetFocusedWindow();
		else
			return nullptr;
	}

	Window* Window::WindowFromPoint(Point pos)
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::WindowFromPoint(pos);
		else
			return nullptr;
	}

	std::unique_ptr<Window> Window::Create(WindowSpecification specs)
	{
		if constexpr (Core::s_IsSystemWindows)
			return std::make_unique<Windows::Win32Window>(std::move(specs));
		else
			return nullptr;
	}

	void Window::MsgLoop()
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::Win32Window::MsgLoop();
	}

	void Window::FatalErrorBox(std::string_view message, std::string_view title, const Utils::BackTrace& backTrace)
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::Win32Window::FatalErrorBox(message, title, backTrace);
	}
} // namespace Brainfryer