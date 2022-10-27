#pragma once

#include "Brainfryer/Input/ButtonStates.h"
#include "Brainfryer/Utils/BackTrace.h"
#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Utils/Event.h"
#include "Brainfryer/Utils/Flags.h"
#include "Brainfryer/Utils/Point.h"
#include "Brainfryer/Utils/Rect.h"

#include <cstddef>
#include <cstdint>

#include <functional>
#include <memory>
#include <string>

namespace Brainfryer
{
	struct Monitor;

	using EWindowFlags = Utils::Flags<>;

	namespace WindowFlags
	{
		static constexpr EWindowFlags None          = 0x0;
		static constexpr EWindowFlags NoDecoration  = 0x1;
		static constexpr EWindowFlags NoTaskBarIcon = 0x2;
		static constexpr EWindowFlags TopMost       = 0x4;
		static constexpr EWindowFlags AlphaSupport  = 0x8;
	} // namespace WindowFlags

	enum class ECursor
	{
		Hidden = 0,
		Arrow,
		IBeam,
		SizeAll,
		SizeWE,
		SizeNS,
		SizeNESW,
		SizeNWSE,
		Hand,
		No
	};

	enum class ECursorMode
	{
		Normal,
		Disabled,
		Raw
	};

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
		EWindowFlags flags   = WindowFlags::None;
	};

	class Window
	{
	public:
		static const std::vector<Monitor>& GetMonitors();

		static bool GetKeyState(std::uint32_t keycode);

		static Point GetCursorPos();
		static void  SetCursor(ECursor cursor);

		static Window* GetFocusedWindow();
		static Window* WindowFromPoint(Point pos);

		static std::unique_ptr<Window> Create(WindowSpecification specs);
		static void                    MsgLoop();

		static void FatalErrorBox(std::string_view message, std::string_view title = "", const Utils::BackTrace& backTrace = {});

	public:
		virtual ~Window() = default;

		virtual void setTitle(std::string title)         = 0;
		virtual void setFlags(EWindowFlags flags)        = 0;
		virtual void setPos(Point pos)                   = 0;
		virtual void setSize(Size size)                  = 0;
		virtual void setRect(Rect rect)                  = 0;
		virtual void restore()                           = 0;
		virtual void iconify()                           = 0;
		virtual void maximize()                          = 0;
		virtual void fullscreen(bool fullsscreen = true) = 0;
		virtual void hide()                              = 0;
		virtual void show(bool activate = true)          = 0;
		virtual void focus()                             = 0;
		virtual void requestClose(bool request = true)   = 0;
		virtual void setAlpha(float alpha)               = 0;
		virtual void setCursorMode(ECursorMode mode)     = 0;

		virtual bool             initialized() const    = 0;
		virtual std::string_view title() const          = 0;
		virtual EWindowFlags     flags() const          = 0;
		virtual Point            pos() const            = 0;
		virtual Size             size() const           = 0;
		virtual Rect             rect() const           = 0;
		virtual EWindowState     state() const          = 0;
		virtual bool             visible() const        = 0;
		virtual bool             requestedClose() const = 0;
		virtual bool             focused() const        = 0;
		virtual float            getDPIScale() const    = 0;
		virtual ECursorMode      getCursorMode() const  = 0;

		virtual Point screenToClient(Point pos) const = 0;
		virtual void  setCursorPos(Point pos)         = 0;

	public:
		Utils::Event<Window*, std::int32_t, std::int32_t>   e_Move;
		Utils::Event<Window*, std::uint32_t, std::uint32_t> e_Size;
		Utils::Event<Window*, EWindowState>                 e_State;
		Utils::Event<Window*, bool>                         e_Focus;
		Utils::Event<Window*, bool>                         e_Visible;
		Utils::Event<Window*, float>                        e_DPIScale;

		Utils::Event<Window*, float, float>                                      e_MouseMove;
		Utils::Event<Window*, bool>                                              e_MouseEnterExit;
		Utils::Event<Window*, float, float>                                      e_MouseScroll;
		Utils::Event<Window*, std::uint32_t, Input::EButtonState>                e_MouseButton;
		Utils::Event<Window*, std::uint32_t, std::uint32_t, Input::EButtonState> e_Key;
		Utils::Event<Window*, std::uint32_t>                                     e_Char;
	};

	struct Monitor
	{
	public:
		Rect  mainArea;
		Rect  workArea;
		float dpiScale;
	};
} // namespace Brainfryer