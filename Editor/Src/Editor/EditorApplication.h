#pragma once

#include "EditorWindow.h"
#include "EditorWindowType.h"

#include <Brainfryer/Renderer/DescriptorHeap.h>
#include <Brainfryer/Renderer/Swapchain.h>
#include <Brainfryer/Window/Window.h>

#include <memory>

namespace Brainfryer::Editor
{
	class EditorApplication
	{
	public:
		bool init();
		void deinit();
		void render();
		void present();

		bool running();
		auto window() const { return m_Window.get(); }

		UID newViewport();

		template <EditorWindowT T, class... Args>
		UID newWindow(Args&&... args)
		{
			UID id = newWindowID();
			m_NewWindows.emplace_back(id, std::make_unique<T>(this, id, std::forward<Args>(args)...));
			return id;
		}
		void removeWindow(UID id);

		auto descriptorHeap() { return m_DescriptorHeap.get(); }

	private:
		UID newWindowID() const;

	private:
		std::unique_ptr<Window>         m_Window;
		std::unique_ptr<Swapchain>      m_Swapchain;
		std::unique_ptr<DescriptorHeap> m_DescriptorHeap;

		bool m_ShowDemoWindow = true;
		//float m_MenuBarHeight  = 0.0f;

		std::unordered_map<UID, std::unique_ptr<EditorWindow>> m_Windows;

		std::vector<std::pair<UID, std::unique_ptr<EditorWindow>>> m_NewWindows;
		std::vector<UID>                                           m_ClosedWindows;
	};
} // namespace Brainfryer::Editor