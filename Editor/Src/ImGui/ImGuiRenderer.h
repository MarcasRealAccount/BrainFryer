#pragma once

#include <Brainfryer/Renderer/CommandList.h>
#include <Brainfryer/Renderer/DescriptorHeap.h>
#include <Brainfryer/Renderer/Swapchain.h>

#include <imgui.h>

namespace Brainfryer::Editor
{
	void ImGuiRendererInit(Swapchain* swapchain, DescriptorHeap* descriptorHeap);
	void ImGuiRendererShutdown();
	void ImGuiRendererNewFrame();
	void ImGuiRendererDrawData(ImDrawData* drawData, CommandList* commandList);

	ImTextureID       ImGuiImage(const DescriptorHeapRef& ref);
	DescriptorHeapRef ImGuiImageRef(ImTextureID id);
} // namespace Brainfryer::Editor