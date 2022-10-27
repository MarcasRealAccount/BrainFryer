#pragma once

namespace Brainfryer
{
	class Window;

	namespace Editor
	{
		void ImGuiBackendInit(Window* window);
		void ImGuiBackendShutdown();
		void ImGuiBackendNewFrame();
	} // namespace Editor
} // namespace Brainfryer