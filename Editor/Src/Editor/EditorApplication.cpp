#include "EditorApplication.h"
#include "EditorViewport.h"
#include "ImGui/ImGuiBackend.h"
#include "ImGui/ImGuiRenderer.h"
#include <Brainfryer/Envs/Windows/Win32Window.h>
#include <Brainfryer/Renderer/Context.h>

#include <imgui.h>

namespace Brainfryer::Editor
{
	bool EditorApplication::init()
	{
		{
			WindowSpecification specs {};
			specs.title   = "BrainFryer Editor";
			specs.state   = EWindowState::Maximized;
			specs.visible = false;
			m_Window      = Window::Create(std::move(specs));
		}
		if (!m_Window->initialized())
			return false;

		{
			SwapchainInfo swapchainInfo {};
			swapchainInfo.window      = m_Window.get();
			swapchainInfo.bufferCount = 3;

			m_Swapchain = Swapchain::Create(swapchainInfo);
		}
		if (!m_Swapchain->initialized())
			return false;

		{
			DescriptorHeapInfo descriptorHeapInfo {};
			descriptorHeapInfo.type          = EDescriptorHeapType::SRV_CBV_UAV;
			descriptorHeapInfo.capacity      = 512;
			descriptorHeapInfo.shaderVisible = true;

			m_DescriptorHeap = DescriptorHeap::Create(descriptorHeapInfo);
		}
		if (!m_DescriptorHeap->initialized())
			return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding              = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		ImGuiBackendInit(m_Window.get());
		ImGuiRendererInit(m_Swapchain.get(), m_DescriptorHeap.get());

		auto itr = m_Windows.insert_or_assign({ EditorWindowType::Viewport, 0 }, std::make_unique<EditorViewport>(this, UID { EditorWindowType::Viewport, 0 })).first;
		if (!itr->second->init())
			return false;

		/*if constexpr (Core::s_IsSystemWindows)
		{
			if (Window::GetAPI() == EWindowingAPI::Win32)
			{
				using namespace Windows;
				Win32Window* wnd = reinterpret_cast<Win32Window*>(m_Window.get());
				SetWindowLongW(wnd->handle(), GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

				MARGINS shadowMargins = { 1, 1, 1, 1 };
				DwmExtendFrameIntoClientArea(wnd->handle(), &shadowMargins);

				wnd->e_WindowMsg += [&](LRESULT Ret, Win32Window* window, UINT Msg, WPARAM wParam, LPARAM lParam) -> bool
				{
					switch (Msg)
					{
					case WM_NCHITTEST:
					{
						std::int32_t x = GET_X_LPARAM(lParam);
						std::int32_t y = GET_Y_LPARAM(lParam);

						auto rect = window->rect();
						if (x >= rect.x && x <= static_cast<std::int32_t>(rect.x + rect.w) &&
						    y >= rect.y && y <= static_cast<std::int32_t>(rect.y + rect.h))
						{
							std::int32_t borderX = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
							std::int32_t borderY = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

							if (y < rect.y + borderY)
							{
								if (x < rect.x + borderX)
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
									Ret = HTTOPLEFT;
									return true;
								}
								else if (x >= static_cast<std::int32_t>(rect.x + rect.w - borderX))
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
									Ret = HTTOPRIGHT;
									return true;
								}
								else
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
									Ret = HTTOP;
									return true;
								}
							}
							else if (y >= static_cast<std::int32_t>(rect.y + rect.h - borderY))
							{
								if (x < rect.x + borderX)
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
									Ret = HTBOTTOMLEFT;
									return true;
								}
								else if (x >= static_cast<std::int32_t>(rect.x + rect.w - borderX))
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
									Ret = HTBOTTOMRIGHT;
									return true;
								}
								else
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
									Ret = HTBOTTOM;
									return true;
								}
							}
							else if (x < rect.x + borderX)
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
								return HTLEFT;
							}
							else if (x < static_cast<std::int32_t>(rect.x + rect.w - borderX))
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
								return HTRIGHT;
							}
							else if (!ImGui::IsAnyItemHovered() && y < rect.y + 30 + static_cast<std::int32_t>(m_MenuBarHeight))
							{
								return HTCAPTION;
							}
						}
						return false;
					}
					case WM_NCCALCSIZE:
					{
						if (wParam == 1)
						{
							WINDOWPLACEMENT windowPlacement {};
							windowPlacement.length = sizeof(WINDOWPLACEMENT);

							if (GetWindowPlacement(window->handle(), &windowPlacement) && windowPlacement.showCmd == SW_SHOWMINIMIZED)
							{
								NCCALCSIZE_PARAMS* params  = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
								std::int32_t       borderX = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
								std::int32_t       borderY = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

								params->rgrc[0].left += borderX;
								params->rgrc[0].top += borderY;
								params->rgrc[0].right -= borderX;
								params->rgrc[0].bottom -= borderY;

								Ret = WVR_VALIDRECTS;
								return true;
							}
						}
						Ret = 0;
						return true;
					}
					}
					return false;
				};
			}
		}*/

		m_Window->show();
		return true;
	}

	void EditorApplication::deinit()
	{
		for (auto& window : m_Windows)
			window.second->deinit();
		m_Windows.clear();
		m_ClosedWindows.clear();

		ImGuiRendererShutdown();
		ImGuiBackendShutdown();

		m_DescriptorHeap.reset();
		m_Swapchain.reset();
		m_Window.reset();
	}

	void EditorApplication::render()
	{
		for (auto& window : m_Windows)
			window.second->update();

		ImGuiIO& io = ImGui::GetIO();

		ImGuiRendererNewFrame();
		ImGuiBackendNewFrame();
		ImGui::NewFrame();

		ImGuiViewport* MainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(MainViewport->Pos);
		ImGui::SetNextWindowSize(MainViewport->Size);
		ImGui::SetNextWindowViewport(MainViewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

		ImGuiWindowFlags DockspaceWindowFlags = ImGuiWindowFlags_MenuBar |
		                                        ImGuiWindowFlags_NoDocking |
		                                        ImGuiWindowFlags_NoTitleBar |
		                                        ImGuiWindowFlags_NoCollapse |
		                                        ImGuiWindowFlags_NoResize |
		                                        ImGuiWindowFlags_NoMove |
		                                        ImGuiWindowFlags_NoBringToFrontOnFocus |
		                                        ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("MainDockspaceWindow", nullptr, DockspaceWindowFlags);
		ImGui::PopStyleVar(3);

		ImGui::DockSpace(ImGui::GetID("MainDockspace"));
		if (ImGui::BeginMainMenuBar())
		{
			//m_MenuBarHeight = ImGui::GetWindowHeight();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Show Demo Window"))
					m_ShowDemoWindow = true;
				if (ImGui::MenuItem("New viewport"))
					newViewport();
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		for (auto& window : m_Windows)
			window.second->render();

		if (m_ShowDemoWindow)
			ImGui::ShowDemoWindow(&m_ShowDemoWindow);

		ImGui::End();

		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::UpdatePlatformWindows();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			for (int i = 1; i < platform_io.Viewports.Size; i++)
			{
				ImGuiViewport* viewport = platform_io.Viewports[i];
				if (viewport->Flags & ImGuiViewportFlags_Minimized)
					continue;
				if (platform_io.Platform_RenderWindow) platform_io.Platform_RenderWindow(viewport, nullptr);
				if (platform_io.Renderer_RenderWindow) platform_io.Renderer_RenderWindow(viewport, nullptr);
			}
		}

		auto commandList = Context::CurrentCMDList();
		commandList->setDescriptorHeaps({ m_DescriptorHeap.get() });
		m_Swapchain->bind(commandList);
		m_Swapchain->clear(commandList, 0.0f, 0.0f, 0.0f, 1.0f);
		ImGuiRendererDrawData(ImGui::GetDrawData(), commandList);
		m_Swapchain->unbind(commandList);
	}

	void EditorApplication::present()
	{
		m_Swapchain->present();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			for (int i = 1; i < platform_io.Viewports.Size; i++)
			{
				ImGuiViewport* viewport = platform_io.Viewports[i];
				if (viewport->Flags & ImGuiViewportFlags_Minimized)
					continue;
				if (platform_io.Platform_SwapBuffers) platform_io.Platform_SwapBuffers(viewport, nullptr);
				if (platform_io.Renderer_SwapBuffers) platform_io.Renderer_SwapBuffers(viewport, nullptr);
			}
		}

		for (auto& viewport : m_NewWindows)
		{
			if (!viewport.second->init())
				continue;

			m_Windows.insert_or_assign(viewport.first, std::move(viewport.second));
		}
		m_NewWindows.clear();

		for (auto viewport : m_ClosedWindows)
		{
			auto itr = m_Windows.find(viewport);
			if (itr != m_Windows.end())
			{
				itr->second->deinit();
				m_Windows.erase(itr);
			}
		}
		m_ClosedWindows.clear();
	}

	bool EditorApplication::running()
	{
		return !m_Window->requestedClose();
	}

	UID EditorApplication::newViewport()
	{
		UID id = newWindowID();
		m_NewWindows.emplace_back(id, std::make_unique<EditorViewport>(this, id));
		return id;
	}

	void EditorApplication::removeWindow(UID id)
	{
		if (id.type() == EditorWindowType::Viewport && id.id() == 0)
			return;

		m_ClosedWindows.emplace_back(id);
	}

	UID EditorApplication::newWindowID() const
	{
		UID id = UID::Random(EditorWindowType::Viewport);
		while (m_Windows.contains(id))
			id = UID::Random(EditorWindowType::Viewport);
		return id;
	}
} // namespace Brainfryer::Editor