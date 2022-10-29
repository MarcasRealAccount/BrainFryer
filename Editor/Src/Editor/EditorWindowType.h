#pragma once

#include <cstdint>

namespace Brainfryer::Editor
{
	namespace EditorWindowType
	{
		static constexpr std::uint16_t Viewport = 0;
	}

	template <class T>
	concept EditorWindowT =
	    requires {
		    {
			    T::WindowType
			    } -> std::convertible_to<std::uint16_t>;
	    };
} // namespace Brainfryer::Editor