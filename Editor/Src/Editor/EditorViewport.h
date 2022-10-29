#pragma once

#include "EditorWindow.h"
#include "EditorWindowType.h"

#include <Brainfryer/Renderer/DescriptorHeapRef.h>
#include <Brainfryer/Renderer/Image.h>
#include <Brainfryer/Renderer/RenderTargets.h>
#include <Brainfryer/Utils/UID.h>

namespace Brainfryer::Editor
{
	class EditorApplication;

	class EditorViewport : public EditorWindow
	{
	public:
		static constexpr std::uint16_t WindowType = EditorWindowType::Viewport;

	public:
		EditorViewport(EditorApplication* application, UID id);

		virtual bool init() override;
		virtual void deinit() override;

	private:
		virtual void onUpdate() override;
		virtual void onRender() override;

	private:
		std::unique_ptr<FrameImage>    m_Image;
		std::unique_ptr<FrameImage>    m_DepthImage;
		std::unique_ptr<RenderTargets> m_RenderTargets;

		std::vector<DescriptorHeapRef> m_PresentImageRefs;
	};
} // namespace Brainfryer::Editor