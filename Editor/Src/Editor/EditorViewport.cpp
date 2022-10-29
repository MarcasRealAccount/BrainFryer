#include "EditorViewport.h"
#include "EditorApplication.h"
#include "ImGui/ImGuiRenderer.h"
#include <Brainfryer/Renderer/Context.h>

#include <imgui.h>

#include <algorithm>

namespace Brainfryer::Editor
{
	EditorViewport::EditorViewport(EditorApplication* application, UID id)
	    : EditorWindow(application, id, "Viewport") {}

	bool EditorViewport::init()
	{
		auto viewportSize = size();
		{
			FrameImageInfo imageInfo {};
			imageInfo.type         = EImageType::_2D;
			imageInfo.format       = EFormat::R16G16B16A16_FLOAT;
			imageInfo.initialState = ImageState::RenderTarget;
			imageInfo.width        = static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.w + 100, 512));
			imageInfo.height       = static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.h + 100, 512));
			imageInfo.depth        = 1;
			imageInfo.flags        = ImageFlags::AllowRenderTarget;
			imageInfo.clear        = { 0.0f, 0.0f, 0.0f, 1.0f };

			m_Image = FrameImage::Create(imageInfo);
		}
		if (!m_Image->initialized())
			return false;

		{
			FrameImageInfo imageInfo {};
			imageInfo.type         = EImageType::_2D;
			imageInfo.format       = EFormat::D24_UNORM_S8_UINT;
			imageInfo.initialState = ImageState::DepthWrite;
			imageInfo.width        = static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.w + 100, 512));
			imageInfo.height       = static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.h + 100, 512));
			imageInfo.depth        = 1;
			imageInfo.flags        = ImageFlags::AllowDepthStencil;
			imageInfo.clear.ds     = { 0.0f, static_cast<std::int16_t>(0) };

			m_DepthImage = FrameImage::Create(imageInfo);
		}
		if (!m_DepthImage->initialized())
		{
			m_Image.reset();
			return false;
		}

		{
			RenderTargetsInfo renderTargetsInfo {};
			renderTargetsInfo.colorViews.emplace_back(m_Image.get(), EImageType::_2D, 0, 0, 0, 1, 0, 0.0f);
			renderTargetsInfo.depthStencil.image           = m_DepthImage.get();
			renderTargetsInfo.depthStencil.type            = EImageType::_2D;
			renderTargetsInfo.depthStencil.mostDetailedMip = 0;
			renderTargetsInfo.depthStencil.mipLevels       = 0;
			renderTargetsInfo.depthStencil.firstArraySlice = 0;
			renderTargetsInfo.depthStencil.arraySize       = 1;
			renderTargetsInfo.depthStencil.planeSlice      = 0;
			renderTargetsInfo.depthStencil.minLODClamp     = 0.0f;

			m_RenderTargets = RenderTargets::Create(renderTargetsInfo);
		}
		if (!m_RenderTargets->initialized())
		{
			m_Image.reset();
			m_DepthImage.reset();
			return false;
		}

		m_PresentImageRefs = application()->descriptorHeap()->createFrameImageViews({ m_Image.get(), EImageType::_2D, 0, 1, 0, 1, 0, 0.0f });
		return true;
	}

	void EditorViewport::deinit()
	{
		m_RenderTargets.reset();
		m_Image.reset();
		m_DepthImage.reset();
	}

	void EditorViewport::onUpdate()
	{
		std::uint32_t frameIndex = Context::FrameIndex();

		auto viewportSize = size();
		auto imageSize    = m_Image->size(frameIndex);
		if ((viewportSize.w > 512 ||
		     viewportSize.h > 512) &&
		    (viewportSize.w < imageSize.w / 2 ||
		     viewportSize.h < imageSize.h / 2 ||
		     viewportSize.w > imageSize.w ||
		     viewportSize.h > imageSize.h))
		{
			m_Image->resize(frameIndex,
			                static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.w + 100, 512)),
			                static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.h + 100, 512)));
			m_DepthImage->resize(frameIndex,
			                     static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.w + 100, 512)),
			                     static_cast<std::uint16_t>(std::max<std::uint32_t>(viewportSize.h + 100, 512)));
			m_RenderTargets->update(frameIndex);

			m_PresentImageRefs[frameIndex] = application()->descriptorHeap()->createFrameImageViews({ m_Image.get(), EImageType::_2D, 0, 1, 0, 1, 0, 0.0f }, frameIndex);
		}
	}

	void EditorViewport::onRender()
	{
		std::uint32_t frameIndex = Context::FrameIndex();

		auto commandList = Context::CurrentCMDList();
		m_Image->transition(commandList, frameIndex, ImageState::RenderTarget);
		m_DepthImage->transition(commandList, frameIndex, ImageState::DepthWrite);
		commandList->bindRenderTargets(m_RenderTargets.get(), frameIndex);

		// TODO(MarcasRealAccount): Render viewport

		m_Image->transition(commandList, frameIndex, ImageState::PixelShaderResource);
		//m_DepthImage->transition(commandList, frameIndex, ImageState::PixelShaderResource);

		auto viewportSize = size();
		ImGui::Image(ImGuiImage(m_PresentImageRefs[frameIndex]),
		             { static_cast<float>(viewportSize.w),
		               static_cast<float>(viewportSize.h) });
	}
} // namespace Brainfryer::Editor