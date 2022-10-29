#pragma once

#include "Brainfryer/Utils/BackTrace.h"
#include "Brainfryer/Utils/Core.h"
#include "Brainfryer/Window/Window.h"
#include "Win32.h"

#include <source_location>

namespace Brainfryer::Windows
{
	LRESULT Win32WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	class Win32Window : public Window
	{
	public:
		friend LRESULT Win32WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

		static const std::vector<Monitor>& GetMonitors();

		static bool GetKeyState(std::uint32_t keycode);

		static Point GetCursorPos();

		static Window* GetFocusedWindow();
		static Window* WindowFromPoint(Point pos);

		static void MsgLoop();
		static void SetMainOnRender(std::function<void()> mainOnRender);

		static void FatalErrorBox(std::string_view message, std::string_view title = "", const Utils::BackTrace& backTrace = {});
		static void HandleLastError(std::string_view functionName);

	public:
		Win32Window(WindowSpecification specs);
		~Win32Window();

		virtual void setTitle(std::string title) override;
		virtual void setFlags(EWindowFlags flags) override;
		virtual void setPos(Point pos) override;
		virtual void setSize(Size size) override;
		virtual void setRect(Rect rect) override;
		virtual void restore() override;
		virtual void iconify() override;
		virtual void maximize() override;
		virtual void fullscreen(bool fullscreen = true) override;
		virtual void hide() override;
		virtual void show(bool activate = true) override;
		virtual void focus() override;
		virtual void requestClose(bool request = true) override;
		virtual void setAlpha(float alpha) override;
		virtual void setCursorMode(ECursorMode mode) override;
		virtual void setCursor(ECursor cursor) override;

		virtual bool             initialized() const override { return !!m_HWnd; }
		virtual std::string_view title() const override { return m_Specs.title; }
		virtual EWindowFlags     flags() const override { return m_Specs.flags; }
		virtual Point            pos() const override { return { m_Specs.rect.x, m_Specs.rect.y }; }
		virtual Size             size() const override { return { m_Specs.rect.w, m_Specs.rect.h }; }
		virtual Rect             rect() const override { return m_Specs.rect; }
		virtual EWindowState     state() const override { return m_Specs.state; }
		virtual bool             visible() const override { return m_Specs.visible; }
		virtual bool             requestedClose() const override { return m_RequestedClose; }
		virtual bool             focused() const override { return m_Focused; }
		virtual float            getDPIScale() const override { return m_DPIScale; }
		virtual ECursorMode      getCursorMode() const override { return m_CursorMode; }
		virtual ECursor          getCursor() const override { return m_Cursor; }

		virtual Point screenToClient(Point pos) const override;
		virtual void  setCursorPos(Point pos) override;

		auto  handle() const { return m_HWnd; }
		auto& getSpecs() const { return m_Specs; }

	public:
		Utils::RequestEvent<Windows::LRESULT, Win32Window*, UINT, WPARAM, LPARAM> e_WindowMsg;

	private:
		WindowSpecification m_Specs;

		EWindowState m_PState;
		EWindowState m_PPState;

		HWND            m_HWnd;
		WINDOWPLACEMENT m_PPlacement;

		bool        m_RequestedClose;
		bool        m_Focused;
		float       m_DPIScale;
		ECursorMode m_CursorMode;
		ECursor     m_Cursor;

		bool          m_CursorTracked = false;
		std::uint16_t m_HighSurrogate = 0;
		Point         m_VirtualCursorPos { 0, 0 };
		Point         m_LastCursorPos { 0, 0 };

		std::vector<std::uint8_t> m_RawInput;
	};
} // namespace Brainfryer::Windows