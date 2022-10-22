#include "Brainfryer/Window/Window.h"
#include "Brainfryer/Envs/Windows/Win32Window.h"

namespace Brainfryer
{
	std::unique_ptr<Window> Window::Create(WindowSpecification specs)
	{
		if constexpr (Core::s_IsSystemWindows)
			return std::make_unique<Windows::Win32Window>(std::move(specs));
		else
			return nullptr;
	}

	void Window::FatalErrorBox(std::string_view message, std::string_view title, const Utils::BackTrace& backTrace)
	{
		if constexpr (Core::s_IsSystemWindows)
			Windows::Win32Window::FatalErrorBox(message, title, backTrace);
	}
} // namespace Brainfryer