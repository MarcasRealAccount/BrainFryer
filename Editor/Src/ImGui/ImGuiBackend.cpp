#include "ImGuiBackend.h"

//#include <Brainfryer/Input/Input.h>
#include <Brainfryer/Utils/Exception.h>
#include <Brainfryer/Window/Window.h>

#include <imgui.h>
#include <spdlog/fmt/fmt.h>

#include <chrono>

namespace Brainfryer::Editor
{
	using Clock = std::chrono::high_resolution_clock;

	static void PlatformInit();
	static void PlatformShutdown();

	struct BackendData
	{
	public:
		Window*           window             = nullptr;
		Clock::time_point time               = Clock::now();
		bool              mouseTracked       = false;
		bool              wantUpdateMonitors = false;
		ImGuiMouseCursor  lastMouseCursor    = ImGuiMouseCursor_COUNT;
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
	}

	void ImGuiBackendShutdown()
	{
		BackendData* bd = GetBackendData();
		Assert(bd, "No platform backend to shutdown, or already shutdown?");
		ImGuiIO& io = ImGui::GetIO();

		PlatformShutdown();

		io.BackendPlatformName     = nullptr;
		io.BackendPlatformUserData = nullptr;
		delete bd;
	}

	static void UpdateMonitors();
	static void UpdateMouseData();
	static void UpdateMouseCursor();

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

		ImGuiMouseCursor mouseCursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (bd->lastMouseCursor != mouseCursor)
		{
			bd->lastMouseCursor = mouseCursor;
			UpdateMouseCursor();
		}
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

		ImGuiID mouseViewportID = 0;
		if (Window* hoveredWindow = Window::WindowFromPoint(mouseScreenPos))
			if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(hoveredWindow))
				mouseViewportID = viewport->ID;
		io.AddMouseViewportEvent(mouseViewportID);
	}

	void UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return;

		ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
		if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			Window::SetCursor(ECursor::Hidden);
		}
		else
		{
			ECursor cursor = ECursor::Arrow;
			switch (imguiCursor)
			{
			case ImGuiMouseCursor_Arrow: cursor = ECursor::Arrow; break;
			case ImGuiMouseCursor_TextInput: cursor = ECursor::IBeam; break;
			case ImGuiMouseCursor_ResizeAll: cursor = ECursor::SizeAll; break;
			case ImGuiMouseCursor_ResizeEW: cursor = ECursor::SizeWE; break;
			case ImGuiMouseCursor_ResizeNS: cursor = ECursor::SizeNS; break;
			case ImGuiMouseCursor_ResizeNESW: cursor = ECursor::SizeNESW; break;
			case ImGuiMouseCursor_ResizeNWSE: cursor = ECursor::SizeNWSE; break;
			case ImGuiMouseCursor_Hand: cursor = ECursor::Hand; break;
			case ImGuiMouseCursor_NotAllowed: cursor = ECursor::No; break;
			}
			Window::SetCursor(cursor);
		}
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
		vp->PlatformHandle = vp->PlatformHandleRaw = vd->window;
	}

	void DestroyWindow(ImGuiViewport* vp)
	{
		ViewportData* vd = GetViewportData(vp);
		if (!vd)
			return;
		vp->PlatformUserData = vp->PlatformHandle = vp->PlatformHandleRaw = nullptr;
		if (vd->ownsWindow)
			delete vd;
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