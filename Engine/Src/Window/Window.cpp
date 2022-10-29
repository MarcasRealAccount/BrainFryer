#include "Brainfryer/Window/Window.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"

namespace Brainfryer
{
	EWindowingAPI Window::GetAPI()
	{
		if constexpr (Core::s_IsSystemWindows)
			return EWindowingAPI::Win32;
		else
			return EWindowingAPI::None;
	}

	const std::vector<Monitor>& Window::GetMonitors()
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetMonitors();
		else
			return {};
	}

	bool Window::GetKeyState(std::uint32_t keycode)
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetKeyState(keycode);
		else
			return false;
	}

	Point Window::GetCursorPos()
	{
		if constexpr (Core::s_IsSystemWindows)
			return Windows::Win32Window::GetCursorPos();
		else
			return { 0, 0 };
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

	void Window::SetMainOnRender(std::function<void()> mainOnRender)
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::Win32Window::SetMainOnRender(std::move(mainOnRender));
	}

	void Window::FatalErrorBox(std::string_view message, std::string_view title, const Utils::BackTrace& backTrace)
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::Win32Window::FatalErrorBox(message, title, backTrace);
	}
} // namespace Brainfryer