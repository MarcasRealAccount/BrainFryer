#include "EditorWindow.h"
#include "EditorApplication.h"

#include <imgui.h>
#include <spdlog/fmt/fmt.h>

namespace Brainfryer::Editor
{
	EditorWindow::EditorWindow(EditorApplication* application, UID id, std::string_view name)
	    : m_Application(application), m_ID(id), m_Size({ 512, 512 })
	{
		m_WindowName = fmt::format("{}##{}", name, m_ID);
	}

	void EditorWindow::update()
	{
		onUpdate();
	}

	void EditorWindow::render()
	{
		bool leaveOpen = true;
		bool isActive  = ImGui::Begin(m_WindowName.c_str(), &leaveOpen);
		if (isActive)
		{
			ImVec2 windowSize = ImGui::GetContentRegionAvail();

			onRender();

			m_Size = { static_cast<std::uint32_t>(windowSize.x),
				       static_cast<std::uint32_t>(windowSize.y) };
		}
		ImGui::End();
		if (!leaveOpen)
			m_Application->removeWindow(m_ID);
	}
} // namespace Brainfryer::Editor