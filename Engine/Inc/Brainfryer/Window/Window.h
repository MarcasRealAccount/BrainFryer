#pragma once

#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Utils/Rect.h"

#include <cstddef>
#include <cstdint>

#include <memory>
#include <string>

namespace Brainfryer
{
	enum class EWindowState
	{
		Normal,
		Iconified,
		Maximized,
		Fullscreen
	};

	struct WindowSpecification
	{
	public:
		std::string  title   = "BrainFryer";
		Rect         rect    = { 1 << 31, 1 << 31, 1280, 720 };
		EWindowState state   = EWindowState::Normal;
		bool         visible = true;
	};

	class Window
	{
	public:
		static std::unique_ptr<Window> Create(WindowSpecification specs);
		static void                    FatalErrorBox(std::string_view message);

	public:
		virtual ~Window() = default;

		virtual void msgLoop() = 0;

		virtual void setTitle(std::string title)         = 0;
		virtual void setWindowRect(Rect rect)            = 0;
		virtual void restore()                           = 0;
		virtual void iconify()                           = 0;
		virtual void maximize()                          = 0;
		virtual void fullscreen(bool fullsscreen = true) = 0;
		virtual void hide()                              = 0;
		virtual void show()                              = 0;
		virtual void requestClose(bool request = true)   = 0;

		virtual bool             initialized() const    = 0;
		virtual std::string_view title() const          = 0;
		virtual Rect             windowRect() const     = 0;
		virtual EWindowState     state() const          = 0;
		virtual bool             visible() const        = 0;
		virtual bool             requestedClose() const = 0;

		void setWindowSize(std::uint32_t width, std::uint32_t height)
		{
			auto rect = windowRect();
			setWindowRect({ rect.x, rect.y, width, height });
		}
		void setWindowPos(std::int32_t x, std::int32_t y)
		{
			auto rect = windowRect();
			setWindowRect({ x, y, rect.w, rect.h });
		}
	};
} // namespace Brainfryer