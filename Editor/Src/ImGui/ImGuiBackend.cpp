#include "ImGuiBackend.h"

#include <Brainfryer/Input/Devices/Keyboard.h>
#include <Brainfryer/Input/Devices/Mouse.h>
#include <Brainfryer/Utils/Exception.h>
#include <Brainfryer/Window/Window.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/fmt/fmt.h>

#include <chrono>

namespace Brainfryer::Editor
{
	using Clock = std::chrono::high_resolution_clock;

	static void PlatformInit();
	static void PlatformShutdown();
	static void UpdateKeyModifiers();
	static void AddKeyEvent(std::uint32_t keycode, std::uint32_t scancode, Input::EButtonState state);

	struct BackendData
	{
	public:
		Window*           window             = nullptr;
		Clock::time_point time               = Clock::now();
		bool              mouseTracked       = false;
		bool              wantUpdateMonitors = false;

		UID focusID;
		UID mouseButtonID;
		UID keyID;
		UID charID;
	};

	static BackendData* GetBackendData()
	{
		return ImGui::GetCurrentContext() ? reinterpret_cast<BackendData*>(ImGui::GetIO().BackendPlatformUserData) : nullptr;
	}

	void ImGuiBackendInit(Window* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		Assert(!io.BackendPlatformUserData, "Already initialized a platform backend!");

		BackendData* bd = new BackendData();
		bd->window      = window;

		io.BackendPlatformName     = "BrainFryer";
		io.BackendPlatformUserData = bd;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

		ImGuiViewport* vp  = ImGui::GetMainViewport();
		vp->PlatformHandle = vp->PlatformHandleRaw = bd->window;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			PlatformInit();

		bd->focusID = bd->window->e_Focus += []([[maybe_unused]] Window* window, bool focused)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddFocusEvent(focused);
		};

		bd->mouseButtonID = bd->window->e_MouseButton += []([[maybe_unused]] Window* window, std::uint32_t button, Input::EButtonState state)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddMouseButtonEvent(button, state == Input::EButtonState::Pressed);
		};

		bd->keyID = bd->window->e_Key += []([[maybe_unused]] Window* window, std::uint32_t keycode, std::uint32_t scancode, Input::EButtonState state)
		{
			UpdateKeyModifiers();

			AddKeyEvent(keycode, scancode, state);
		};

		bd->charID = bd->window->e_Char += []([[maybe_unused]] Window* window, std::uint32_t codepoint)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharacter(codepoint);
		};
	}

	void ImGuiBackendShutdown()
	{
		BackendData* bd = GetBackendData();
		Assert(bd, "No platform backend to shutdown, or already shutdown?");
		ImGuiIO& io = ImGui::GetIO();

		PlatformShutdown();

		io.BackendPlatformName     = nullptr;
		io.BackendPlatformUserData = nullptr;

		bd->window->e_Focus -= bd->focusID;
		bd->window->e_MouseButton -= bd->mouseButtonID;
		bd->window->e_Key -= bd->keyID;
		bd->window->e_Char -= bd->charID;
		delete bd;
	}

	static void UpdateMonitors();
	static void UpdateMouseData();

	void ImGuiBackendNewFrame()
	{
		ImGuiIO&     io = ImGui::GetIO();
		BackendData* bd = GetBackendData();
		Assert(bd, "Did you call ImGuiBackendInit()?");

		auto size      = bd->window->size();
		io.DisplaySize = { static_cast<float>(size.w), static_cast<float>(size.h) };
		if (bd->wantUpdateMonitors)
			UpdateMonitors();

		auto currentTime = Clock::now();
		io.DeltaTime     = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - bd->time).count();
		bd->time         = currentTime;

		UpdateMouseData();
	}

	static EWindowFlags GetWindowFlags(ImGuiViewportFlags flags);
	static void         CreateWindow(ImGuiViewport* vp);
	static void         DestroyWindow(ImGuiViewport* vp);
	static void         UpdateWindow(ImGuiViewport* vp);
	static void         ShowWindow(ImGuiViewport* vp);
	static void         SetWindowTitle(ImGuiViewport* vp, const char* title);
	static void         SetWindowPos(ImGuiViewport* vp, ImVec2 pos);
	static ImVec2       GetWindowPos(ImGuiViewport* vp);
	static void         SetWindowSize(ImGuiViewport* vp, ImVec2 size);
	static ImVec2       GetWindowSize(ImGuiViewport* vp);
	static void         SetWindowFocus(ImGuiViewport* vp);
	static bool         GetWindowFocus(ImGuiViewport* vp);
	static bool         GetWindowMinimized(ImGuiViewport* vp);
	static void         SetWindowAlpha(ImGuiViewport* vp, float alpha);
	static float        GetWindowDPIScale(ImGuiViewport* vp);

	struct ViewportData
	{
		Window*      window;
		EWindowFlags flags;
		bool         ownsWindow;
	};

	ViewportData* GetViewportData(ImGuiViewport* vp)
	{
		return reinterpret_cast<ViewportData*>(vp->PlatformUserData);
	}

	void PlatformInit()
	{
		UpdateMonitors();

		ImGuiPlatformIO& platformIO            = ImGui::GetPlatformIO();
		platformIO.Platform_CreateWindow       = &CreateWindow;
		platformIO.Platform_DestroyWindow      = &DestroyWindow;
		platformIO.Platform_UpdateWindow       = &UpdateWindow;
		platformIO.Platform_ShowWindow         = &ShowWindow;
		platformIO.Platform_SetWindowTitle     = &SetWindowTitle;
		platformIO.Platform_SetWindowPos       = &SetWindowPos;
		platformIO.Platform_GetWindowPos       = &GetWindowPos;
		platformIO.Platform_SetWindowSize      = &SetWindowSize;
		platformIO.Platform_GetWindowSize      = &GetWindowSize;
		platformIO.Platform_SetWindowFocus     = &SetWindowFocus;
		platformIO.Platform_GetWindowFocus     = &GetWindowFocus;
		platformIO.Platform_GetWindowMinimized = &GetWindowMinimized;
		platformIO.Platform_SetWindowAlpha     = &SetWindowAlpha;
		platformIO.Platform_GetWindowDpiScale  = &GetWindowDPIScale;

		ImGuiViewport* mainViewport    = ImGui::GetMainViewport();
		BackendData*   bd              = GetBackendData();
		ViewportData*  vd              = new ViewportData();
		vd->window                     = bd->window;
		vd->flags                      = vd->window->flags();
		vd->ownsWindow                 = false;
		mainViewport->PlatformUserData = vd;
		mainViewport->PlatformHandle = mainViewport->PlatformHandleRaw = bd->window;
	}

	void PlatformShutdown()
	{
		ImGui::DestroyPlatformWindows();
	}

	void UpdateMonitors()
	{
		BackendData*     bd         = GetBackendData();
		auto&            monitors   = Window::GetMonitors();
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		platformIO.Monitors.resize(static_cast<int>(monitors.size()));
		for (int i = 0; i < platformIO.Monitors.size(); ++i)
		{
			auto& monitor      = monitors[i];
			auto& imMonitor    = platformIO.Monitors[i];
			imMonitor.DpiScale = monitor.dpiScale;
			imMonitor.MainPos  = { static_cast<float>(monitor.mainArea.x), static_cast<float>(monitor.mainArea.y) };
			imMonitor.MainSize = { static_cast<float>(monitor.mainArea.w), static_cast<float>(monitor.mainArea.h) };
			imMonitor.WorkPos  = { static_cast<float>(monitor.workArea.x), static_cast<float>(monitor.workArea.y) };
			imMonitor.WorkSize = { static_cast<float>(monitor.workArea.w), static_cast<float>(monitor.workArea.h) };
		}
		bd->wantUpdateMonitors = false;
	}

	static ECursor GetWindowCursor(ImGuiMouseCursor cursor)
	{
		switch (cursor)
		{
		case ImGuiMouseCursor_None: return ECursor::Hidden;
		case ImGuiMouseCursor_Arrow: return ECursor::Arrow;
		case ImGuiMouseCursor_TextInput: return ECursor::IBeam;
		case ImGuiMouseCursor_ResizeAll: return ECursor::SizeAll;
		case ImGuiMouseCursor_ResizeEW: return ECursor::SizeWE;
		case ImGuiMouseCursor_ResizeNS: return ECursor::SizeNS;
		case ImGuiMouseCursor_ResizeNESW: return ECursor::SizeNESW;
		case ImGuiMouseCursor_ResizeNWSE: return ECursor::SizeNWSE;
		case ImGuiMouseCursor_Hand: return ECursor::Hand;
		case ImGuiMouseCursor_NotAllowed: return ECursor::No;
		}
		return ECursor::Arrow;
	}

	void UpdateMouseData()
	{
		BackendData* bd = GetBackendData();
		ImGuiIO&     io = ImGui::GetIO();
		Assert(bd->window, "Missing window!?!");

		Point mouseScreenPos = Window::GetCursorPos();

		Window* focusedWindow = Window::GetFocusedWindow();
		if (focusedWindow && ImGui::FindViewportByPlatformHandle(focusedWindow))
		{
			if (io.WantSetMousePos)
			{
				focusedWindow->setCursorPos(Point { static_cast<std::int32_t>(io.MousePos.x), static_cast<std::int32_t>(io.MousePos.y) });
			}
			else if (!bd->mouseTracked)
			{
				Point mousePos = io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ? mouseScreenPos : bd->window->screenToClient(mouseScreenPos);
				io.AddMousePosEvent(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
			}
		}

		ImGuiViewport* mouseViewport   = nullptr;
		ImGuiID        mouseViewportID = 0;
		if (Window* hoveredWindow = Window::WindowFromPoint(mouseScreenPos))
			if (mouseViewport = ImGui::FindViewportByPlatformHandle(hoveredWindow))
				mouseViewportID = mouseViewport->ID;
		io.AddMouseViewportEvent(mouseViewportID);

		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return;

		if (!mouseViewport)
			mouseViewport = ImGui::GetMainViewport();
		ECursor       mouseCursor = GetWindowCursor(io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor());
		ViewportData* vd          = GetViewportData(mouseViewport);
		if (vd->window->getCursor() != mouseCursor)
			vd->window->setCursor(mouseCursor);
	}

	static ImGuiKey s_KeycodeToImGuiKeycode[512] {
		ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_Space, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_Apostrophe, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_Comma, ImGuiKey_Minus, ImGuiKey_Period, ImGuiKey_Slash, ImGuiKey_0, ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4, ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8, ImGuiKey_9, ImGuiKey_None, ImGuiKey_Semicolon, ImGuiKey_None, ImGuiKey_Equal, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_A, ImGuiKey_B, ImGuiKey_C, ImGuiKey_D, ImGuiKey_E, ImGuiKey_F, ImGuiKey_G, ImGuiKey_H, ImGuiKey_I, ImGuiKey_J, ImGuiKey_K, ImGuiKey_L, ImGuiKey_M, ImGuiKey_N, ImGuiKey_O, ImGuiKey_P, ImGuiKey_Q, ImGuiKey_R, ImGuiKey_S, ImGuiKey_T, ImGuiKey_U, ImGuiKey_V, ImGuiKey_W, ImGuiKey_X, ImGuiKey_Y, ImGuiKey_Z, ImGuiKey_LeftBracket, ImGuiKey_Backslash, ImGuiKey_RightBracket, ImGuiKey_None, ImGuiKey_None, ImGuiKey_GraveAccent, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_Escape, ImGuiKey_Enter, ImGuiKey_Tab, ImGuiKey_Backspace, ImGuiKey_Insert, ImGuiKey_Delete, ImGuiKey_RightArrow, ImGuiKey_LeftArrow, ImGuiKey_DownArrow, ImGuiKey_UpArrow, ImGuiKey_PageUp, ImGuiKey_PageDown, ImGuiKey_Home, ImGuiKey_End, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_CapsLock, ImGuiKey_ScrollLock, ImGuiKey_NumLock, ImGuiKey_PrintScreen, ImGuiKey_Pause, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_F1, ImGuiKey_F2, ImGuiKey_F3, ImGuiKey_F4, ImGuiKey_F5, ImGuiKey_F6, ImGuiKey_F7, ImGuiKey_F8, ImGuiKey_F9, ImGuiKey_F10, ImGuiKey_F11, ImGuiKey_F12, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_Keypad0, ImGuiKey_Keypad1, ImGuiKey_Keypad2, ImGuiKey_Keypad3, ImGuiKey_Keypad4, ImGuiKey_Keypad5, ImGuiKey_Keypad6, ImGuiKey_Keypad7, ImGuiKey_Keypad8, ImGuiKey_Keypad9, ImGuiKey_KeypadDecimal, ImGuiKey_KeypadDivide, ImGuiKey_KeypadMultiply, ImGuiKey_KeypadSubtract, ImGuiKey_KeypadAdd, ImGuiKey_KeypadEnter, ImGuiKey_KeypadEqual, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_LeftShift, ImGuiKey_LeftCtrl, ImGuiKey_LeftAlt, ImGuiKey_LeftSuper, ImGuiKey_RightShift, ImGuiKey_RightCtrl, ImGuiKey_RightAlt, ImGuiKey_RightSuper, ImGuiKey_Menu, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None
	};

	void UpdateKeyModifiers()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiMod_Ctrl, Window::GetKeyState(Input::ButtonIndices::KeyLeftControl) || Window::GetKeyState(Input::ButtonIndices::KeyRightControl));
		io.AddKeyEvent(ImGuiMod_Shift, Window::GetKeyState(Input::ButtonIndices::KeyLeftShift) || Window::GetKeyState(Input::ButtonIndices::KeyRightShift));
		io.AddKeyEvent(ImGuiMod_Alt, Window::GetKeyState(Input::ButtonIndices::KeyLeftAlt) || Window::GetKeyState(Input::ButtonIndices::KeyRightAlt));
		io.AddKeyEvent(ImGuiMod_Super, Window::GetKeyState(Input::ButtonIndices::KeyLeftSuper) || Window::GetKeyState(Input::ButtonIndices::KeyRightSuper));
	}

	void AddKeyEvent(std::uint32_t keycode, std::uint32_t scancode, Input::EButtonState state)
	{
		ImGuiKey key = s_KeycodeToImGuiKeycode[keycode];

		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(key, state == Input::EButtonState::Pressed || state == Input::EButtonState::Repeated);
		io.SetKeyEventNativeData(key, keycode, scancode);
	}

	EWindowFlags GetWindowFlags(ImGuiViewportFlags flags)
	{
		EWindowFlags flgs = WindowFlags::None;
		if (flags & ImGuiViewportFlags_NoDecoration)
			flgs |= WindowFlags::NoDecoration;
		if (flags & ImGuiViewportFlags_NoTaskBarIcon)
			flgs |= WindowFlags::NoTaskBarIcon;
		if (flags & ImGuiViewportFlags_TopMost)
			flgs |= WindowFlags::TopMost;
		return flgs;
	}

	void CreateWindow(ImGuiViewport* vp)
	{
		ViewportData* vd = new ViewportData();

		WindowSpecification specs {};
		specs.title               = fmt::format("ImGui Viewport {}", vp->ID);
		specs.rect                = { static_cast<std::int32_t>(vp->Pos.x), static_cast<std::int32_t>(vp->Pos.y), static_cast<std::uint32_t>(vp->Size.x), static_cast<std::uint32_t>(vp->Size.y) };
		specs.visible             = true;
		specs.flags               = GetWindowFlags(vp->Flags);
		vd->flags                 = specs.flags;
		vd->window                = Window::Create(std::move(specs)).release();
		vd->ownsWindow            = true;
		vp->PlatformRequestResize = false;
		vp->PlatformUserData      = vd;
		vp->PlatformHandle = vp->PlatformHandleRaw = vd->window;

		vd->window->e_Focus += []([[maybe_unused]] Window* window, bool focused)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddFocusEvent(focused);
		};

		vd->window->e_MouseButton += []([[maybe_unused]] Window* window, std::uint32_t button, Input::EButtonState state)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddMouseButtonEvent(button, state == Input::EButtonState::Pressed);
		};

		vd->window->e_Key += []([[maybe_unused]] Window* window, std::uint32_t keycode, std::uint32_t scancode, Input::EButtonState state)
		{
			UpdateKeyModifiers();

			AddKeyEvent(keycode, scancode, state);
		};

		vd->window->e_Char += []([[maybe_unused]] Window* window, std::uint32_t codepoint)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharacter(codepoint);
		};
	}

	void DestroyWindow(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		if (!vd)
			return;
		vp->PlatformUserData = vp->PlatformHandle = vp->PlatformHandleRaw = nullptr;
		if (vd->ownsWindow)
		{
			delete vd->window;
			delete vd;
		}
	}

	void UpdateWindow(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		EWindowFlags flags = GetWindowFlags(vp->Flags);
		if (flags != vd->flags)
		{
			vd->window->setFlags(flags);
			vd->flags               = flags;
			vp->PlatformRequestMove = vp->PlatformRequestResize = true;
		}
	}

	void ShowWindow(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		if (vp->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
			vd->window->show(false);
		else
			vd->window->show();
	}

	void SetWindowTitle(ImGuiViewport* vp, const char* title)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		vd->window->setTitle(title);
	}

	void SetWindowPos(ImGuiViewport* vp, ImVec2 pos)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		vd->window->setPos({ static_cast<std::int32_t>(pos.x), static_cast<std::int32_t>(pos.y) });
	}

	ImVec2 GetWindowPos(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		auto pos = vd->window->pos();
		return { static_cast<float>(pos.x), static_cast<float>(pos.y) };
	}

	void SetWindowSize(ImGuiViewport* vp, ImVec2 size)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		vd->window->setSize({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
	}

	ImVec2 GetWindowSize(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		auto size = vd->window->size();
		return { static_cast<float>(size.w), static_cast<float>(size.h) };
	}

	void SetWindowFocus(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		vd->window->focus();
	}

	bool GetWindowFocus(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		return vd->window->focused();
	}

	bool GetWindowMinimized(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		return vd->window->state() == EWindowState::Iconified;
	}

	void SetWindowAlpha(ImGuiViewport* vp, float alpha)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		vd->window->setAlpha(alpha);
	}

	float GetWindowDPIScale(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		Assert(vd->window, "Missing window");
		return vd->window->getDPIScale();
	}
} // namespace Brainfryer::Editor