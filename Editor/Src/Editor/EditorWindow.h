#pragma once

#include <Brainfryer/Utils/Point.h>
#include <Brainfryer/Utils/UID.h>

#include <string>

namespace Brainfryer::Editor
{
	class EditorApplication;

	class EditorWindow
	{
	public:
		EditorWindow(EditorApplication* application, UID id, std::string_view name);

		virtual bool init()   = 0;
		virtual void deinit() = 0;
		void         update();
		void         render();

		auto application() const { return m_Application; }
		auto size() const { return m_Size; }
		UID  id() const { return m_ID; }

	private:
		virtual void onUpdate() = 0;
		virtual void onRender() = 0;

	private:
		EditorApplication* m_Application;
		UID                m_ID;
		Size               m_Size;

		std::string m_WindowName;
	};
} // namespace Brainfryer::Editor