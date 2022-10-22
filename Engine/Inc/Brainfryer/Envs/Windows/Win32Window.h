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

		static void FatalErrorBox(std::string_view message, std::string_view title = "", const Utils::BackTrace& backTrace = {});
		static void HandleLastError(std::string_view functionName);

	public:
		Win32Window(WindowSpecification specs);
		~Win32Window();

		virtual void msgLoop() override;

		virtual void setTitle(std::string title) override;
		virtual void setWindowRect(Rect rect) override;
		virtual void restore() override;
		virtual void iconify() override;
		virtual void maximize() override;
		virtual void fullscreen(bool fullscreen = true) override;
		virtual void hide() override;
		virtual void show() override;
		virtual void requestClose(bool request = true) override;

		virtual bool             initialized() const override { return !!m_HWnd; }
		virtual std::string_view title() const override;
		virtual Rect             windowRect() const override;
		virtual EWindowState     state() const override;
		virtual bool             visible() const override;
		virtual bool             requestedClose() const override;

		auto  nativeHandle() const { return m_HWnd; }
		auto& getSpecs() const { return m_Specs; }

	private:
		WindowSpecification m_Specs;

		EWindowState m_PState;
		EWindowState m_PPState;

		HWND            m_HWnd;
		WINDOWPLACEMENT m_PPlacement;

		bool m_RequestedClose;
	};
} // namespace Brainfryer::Windows