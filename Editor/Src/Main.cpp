#include "Editor/EditorApplication.h"
#include "Envs/DX12/DXCompiler.h"
#include "ImGui/ImGuiBackend.h"
#include "ImGui/ImGuiRenderer.h"
#include "Utils/Core.h"
#include <Brainfryer/Envs/DX12/DX12Context.h>
#include <Brainfryer/Renderer/Buffer.h>
#include <Brainfryer/Renderer/CommandAllocator.h>
#include <Brainfryer/Renderer/CommandList.h>
#include <Brainfryer/Renderer/Context.h>
#include <Brainfryer/Renderer/DescriptorHeap.h>
#include <Brainfryer/Renderer/GraphicsPipeline.h>
#include <Brainfryer/Renderer/Image.h>
#include <Brainfryer/Renderer/PipelineLayout.h>
#include <Brainfryer/Renderer/RenderTargets.h>
#include <Brainfryer/Renderer/Swapchain.h>
#include <Brainfryer/Utils/Exception.h>
#include <Brainfryer/Utils/Log.h>
#include <Brainfryer/Window/Window.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>

namespace Brainfryer::Editor
{
	void SafeMain([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
	{
		if (!Context::SelectBestAPI())
		{
			Window::FatalErrorBox("Failed to select graphics context");
			return;
		}

		EditorApplication application {};
		if (!application.init())
			return;

		auto onMainRender = [&]()
		{
			auto commandList = Context::NextFrame();
			commandList->begin();

			application.render();

			commandList->end();

			Context::ExecuteCommandLists({ commandList });

			application.present();
		};

		Window::SetMainOnRender(onMainRender);

		while (application.running())
		{
			onMainRender();
			Window::MsgLoop();
		}

		Context::WaitForGPU();
		application.deinit();

		Context::Destroy();

		/*Brainfryer::WindowSpecification windowSpec {};
		std::unique_ptr<Brainfryer::Window> window = Brainfryer::Window::Create(windowSpec);
		if (!Brainfryer::Context::SelectBestAPI())
		{
			Brainfryer::Window::FatalErrorBox("Failed to select context api");
			return 2;
		}
		{
			std::unique_ptr<Brainfryer::Swapchain> swapchain;
			{
				Brainfryer::SwapchainInfo swapchainInfo {};
				swapchainInfo.window      = window.get();
				swapchainInfo.bufferCount = 3;

				swapchain = Brainfryer::Swapchain::Create(swapchainInfo);
			}
			if (!swapchain->initialized())
				return 3;

			std::unique_ptr<Brainfryer::PipelineLayout> scenePipelineLayout;
			std::unique_ptr<Brainfryer::PipelineLayout> blitPipelineLayout;
			{
				Brainfryer::PipelineLayoutInfo pipelineLayoutInfo {};
				pipelineLayoutInfo.flags = Brainfryer::PipelineLayoutFlags::AllowInputAssemblerInputLayout;
				pipelineLayoutInfo.parameters.emplace_back(Brainfryer::PipelineLayoutDescriptorTable { { { Brainfryer::EPipelineLayoutDescriptorRangeType::ShaderResourceView, 1, 0, 0, 0, Brainfryer::PipelineLayoutDescriptorRangeFlags::DataStatic } } }, Brainfryer::EShaderVisibility::Pixel);
				pipelineLayoutInfo.staticSamplers.emplace_back(
				    Brainfryer::EFilter::Nearest,
				    Brainfryer::EFilter::Nearest,
				    Brainfryer::EImageAddressMode::Wrap,
				    Brainfryer::EImageAddressMode::Wrap,
				    Brainfryer::EImageAddressMode::Wrap,
				    0.0f,
				    0,
				    Brainfryer::EComparisonFunc::Never,
				    Brainfryer::EBorderColor::TransparentBlack,
				    0.0f,
				    1.0f,
				    0,
				    0,
				    Brainfryer::EShaderVisibility::Pixel);
				scenePipelineLayout = Brainfryer::PipelineLayout::Create(pipelineLayoutInfo);
				blitPipelineLayout  = Brainfryer::PipelineLayout::Create(std::move(pipelineLayoutInfo));
			}
			if (!scenePipelineLayout->initialized() || !blitPipelineLayout->initialized())
				return 4;

			std::unique_ptr<Brainfryer::GraphicsPipeline> scenePipeline;
			std::unique_ptr<Brainfryer::GraphicsPipeline> blitPipeline;
			{
				std::uint32_t compileFlags = 0;
				if constexpr (Core::s_IsConfigDebug)
					compileFlags |= DX12::D3DCOMPILE_DEBUG | DX12::D3DCOMPILE_SKIP_OPTIMIZATION;

				Brainfryer::GraphicsPipelineInfo pipelineInfo {};
				pipelineInfo.pipelineLayout = scenePipelineLayout.get();

				pipelineInfo.vertexShader.data = DX12::CompileShader(L"scene.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0);
				pipelineInfo.pixelShader.data  = DX12::CompileShader(L"scene.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0);

				pipelineInfo.inputs.emplace_back("POSITION", 0, Brainfryer::EFormat::R32G32B32_FLOAT, 0, 0, 0);
				pipelineInfo.inputs.emplace_back("TEXCOORD", 0, Brainfryer::EFormat::R32G32_FLOAT, 0, 12, 0);

				pipelineInfo.primitiveTopology = Brainfryer::EPrimitiveTopology::Triangles;

				pipelineInfo.renderTargetCount = 1;
				pipelineInfo.rtvFormats[0]     = swapchain->format();

				scenePipeline = Brainfryer::GraphicsPipeline::Create(pipelineInfo);

				pipelineInfo.vertexShader.data = DX12::CompileShader(L"blit.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0);
				pipelineInfo.pixelShader.data  = DX12::CompileShader(L"blit.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0);

				pipelineInfo.inputs.clear();
				pipelineInfo.inputs.emplace_back("POSITION", 0, Brainfryer::EFormat::R32G32_FLOAT, 0, 0, 0);
				pipelineInfo.inputs.emplace_back("TEXCOORD", 0, Brainfryer::EFormat::R32G32_FLOAT, 0, 8, 0);

				blitPipeline = Brainfryer::GraphicsPipeline::Create(pipelineInfo);
			}
			if (!scenePipeline->initialized() || !blitPipeline->initialized())
				return 5;

			std::unique_ptr<Brainfryer::CommandAllocator> commandAllocator;
			{
				Brainfryer::CommandAllocatorInfo info {};
				info.type        = Brainfryer::ECommandListType::Direct;
				commandAllocator = Brainfryer::CommandAllocator::Create(info);
			}
			if (!commandAllocator->initialized())
				return 6;

			auto loadCommandLists = commandAllocator->allocate(1);
			if (loadCommandLists.empty())
				return 6;
			auto& loadCommandList = loadCommandLists[0];
			loadCommandList->begin(nullptr);

			std::unique_ptr<Brainfryer::Buffer> vertexBuffer;
			std::unique_ptr<Brainfryer::Buffer> indexBuffer;
			std::unique_ptr<Brainfryer::Image>  image;
			std::unique_ptr<Brainfryer::Buffer> blitVertexBuffer;
			std::unique_ptr<Brainfryer::Buffer> stagingVertexBuffer;
			std::unique_ptr<Brainfryer::Buffer> stagingIndexBuffer;
			std::unique_ptr<Brainfryer::Buffer> stagingImageBuffer;
			std::unique_ptr<Brainfryer::Buffer> stagingBlitVertexBuffer;
			Brainfryer::VertexBufferView        vertexBufferView {};
			Brainfryer::IndexBufferView         indexBufferView {};
			Brainfryer::VertexBufferView        blitVertexBufferView {};
			{
				float blitVertices[] {
					1.0f, -1.0f, 1.0f, 1.0f,   1
					-1.0f, -1.0f, 0.0f, 1.0f,  0
					-1.0f, 1.0f, 0.0f, 0.0f,   3
					1.0f, 1.0f, 1.0f, 0.0f,    2
					1.0f, -1.0f, 1.0f, 1.0f,   1
					-1.0f, 1.0f, 0.0f, 0.0f    3
				};

				float triangleVertices[] {
					-0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
					0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
					0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
					-0.5f, 0.5f, 0.0f, 0.0f, 0.0f
				};

				std::uint32_t triangleIndices[] {
					1, 0, 3,
					2, 1, 3
				};

				{
					Brainfryer::BufferInfo bufferInfo {};
					bufferInfo.heapType     = Brainfryer::EHeapType::Default;
					bufferInfo.initialState = Brainfryer::BufferState::CopyDst;
					bufferInfo.alignment    = 0;
					bufferInfo.size         = sizeof(blitVertices);

					Brainfryer::BufferInfo stagingBufferInfo {};
					stagingBufferInfo.heapType  = Brainfryer::EHeapType::Upload;
					stagingBufferInfo.alignment = 0;
					stagingBufferInfo.size      = sizeof(blitVertices);

					blitVertexBuffer        = Brainfryer::Buffer::Create(bufferInfo);
					stagingBlitVertexBuffer = Brainfryer::Buffer::Create(stagingBufferInfo);
					if (!blitVertexBuffer->initialized() || !stagingBlitVertexBuffer->initialized())
						return 7;

					void* memory = stagingBlitVertexBuffer->map();
					std::memcpy(memory, blitVertices, sizeof(blitVertices));
					stagingBlitVertexBuffer->unmap();

					blitVertexBuffer->copyFrom(loadCommandList.get(), { stagingBlitVertexBuffer.get(), 0, sizeof(blitVertices) });
					blitVertexBuffer->transition(loadCommandList.get(), Brainfryer::BufferState::VertexAndConstant);
				}

				{
					Brainfryer::BufferInfo bufferInfo {};
					bufferInfo.heapType     = Brainfryer::EHeapType::Default;
					bufferInfo.initialState = Brainfryer::BufferState::CopyDst;
					bufferInfo.alignment    = 0;
					bufferInfo.size         = sizeof(triangleVertices);

					Brainfryer::BufferInfo stagingBufferInfo {};
					stagingBufferInfo.heapType  = Brainfryer::EHeapType::Upload;
					stagingBufferInfo.alignment = 0;
					stagingBufferInfo.size      = sizeof(triangleVertices);

					vertexBuffer        = Brainfryer::Buffer::Create(bufferInfo);
					stagingVertexBuffer = Brainfryer::Buffer::Create(stagingBufferInfo);
					if (!vertexBuffer->initialized() || !stagingVertexBuffer->initialized())
						return 7;

					void* memory = stagingVertexBuffer->map();
					std::memcpy(memory, triangleVertices, sizeof(triangleVertices));
					stagingVertexBuffer->unmap();

					vertexBuffer->copyFrom(loadCommandList.get(), { stagingVertexBuffer.get(), 0, sizeof(triangleVertices) });
					vertexBuffer->transition(loadCommandList.get(), Brainfryer::BufferState::VertexAndConstant);
				}

				{
					Brainfryer::BufferInfo bufferInfo {};
					bufferInfo.heapType     = Brainfryer::EHeapType::Default;
					bufferInfo.initialState = Brainfryer::BufferState::CopyDst;
					bufferInfo.alignment    = 0;
					bufferInfo.size         = sizeof(triangleIndices);

					Brainfryer::BufferInfo stagingBufferInfo {};
					stagingBufferInfo.heapType  = Brainfryer::EHeapType::Upload;
					stagingBufferInfo.alignment = 0;
					stagingBufferInfo.size      = sizeof(triangleIndices);

					indexBuffer        = Brainfryer::Buffer::Create(bufferInfo);
					stagingIndexBuffer = Brainfryer::Buffer::Create(stagingBufferInfo);
					if (!indexBuffer->initialized() || !stagingIndexBuffer->initialized())
						return 8;

					void* memory = stagingIndexBuffer->map();
					std::memcpy(memory, triangleIndices, sizeof(triangleIndices));
					stagingIndexBuffer->unmap();

					indexBuffer->copyFrom(loadCommandList.get(), { stagingIndexBuffer.get(), 0, sizeof(triangleIndices) });
					indexBuffer->transition(loadCommandList.get(), Brainfryer::BufferState::Index);
				}

				{
					int  w, h, comps;
					auto imageRes = stbi_load("SeenSomeShit.png", &w, &h, &comps, 4);

					Brainfryer::ImageInfo imageInfo {};
					imageInfo.type         = Brainfryer::EImageType::_2D;
					imageInfo.format       = Brainfryer::EFormat::R8G8B8A8_UNORM;
					imageInfo.initialState = Brainfryer::ImageState::CopyDst;
					imageInfo.width        = static_cast<std::uint16_t>(w);
					imageInfo.height       = static_cast<std::uint16_t>(h);
					imageInfo.depth        = 1;
					imageInfo.flags        = Brainfryer::ImageFlags::None;

					image = Brainfryer::Image::Create(imageInfo);
					if (!image->initialized())
						return 9;

					Brainfryer::BufferInfo bufferInfo {};
					bufferInfo.heapType  = Brainfryer::EHeapType::Upload;
					bufferInfo.alignment = 0;
					bufferInfo.size      = image->requiredBufferSize();

					stagingImageBuffer = Brainfryer::Buffer::Create(bufferInfo);
					if (!stagingImageBuffer->initialized())
						return 9;

					void* memory = stagingImageBuffer->map();
					std::memcpy(memory, imageRes, w * h * 4);
					stagingImageBuffer->unmap();

					image->copyFrom(loadCommandList.get(), { stagingImageBuffer.get(), 0, Brainfryer::EFormat::R8G8B8A8_UNORM, static_cast<std::uint16_t>(w), static_cast<std::uint16_t>(h), 1, static_cast<std::uint32_t>(w) * 4 });
					image->transition(loadCommandList.get(), Brainfryer::ImageState::PixelShaderResource);
					stbi_image_free(imageRes);
				}

				vertexBufferView.buffer = vertexBuffer.get();
				vertexBufferView.offset = 0;
				vertexBufferView.size   = sizeof(triangleVertices);
				vertexBufferView.stride = 20;

				indexBufferView.buffer = indexBuffer.get();
				indexBufferView.offset = 0;
				indexBufferView.size   = sizeof(triangleIndices);
				indexBufferView.format = Brainfryer::EFormat::R32_UINT;

				blitVertexBufferView.buffer = blitVertexBuffer.get();
				blitVertexBufferView.offset = 0;
				blitVertexBufferView.size   = sizeof(blitVertices);
				blitVertexBufferView.stride = 16;
			}

			std::unique_ptr<Brainfryer::RenderTargets> renderTargets;
			std::unique_ptr<Brainfryer::FrameImage>    renderTarget;
			{
				Brainfryer::FrameImageInfo renderTargetInfo {};
				renderTargetInfo.type         = Brainfryer::EImageType::_2D;
				renderTargetInfo.format       = Brainfryer::EFormat::R8G8B8A8_UNORM;
				renderTargetInfo.initialState = Brainfryer::ImageState::RenderTarget;
				renderTargetInfo.width        = 360;
				renderTargetInfo.height       = 360;
				renderTargetInfo.depth        = 1;
				renderTargetInfo.flags        = Brainfryer::ImageFlags::AllowRenderTarget;
				renderTargetInfo.clear        = { 0.1f, 0.1f, 0.1f, 1.0f };

				renderTarget = Brainfryer::FrameImage::Create(renderTargetInfo);
				if (!renderTarget->initialized())
					return 10;

				Brainfryer::RenderTargetsInfo renderTargetsInfo {};
				renderTargetsInfo.colorViews.emplace_back(Brainfryer::FrameImageView { renderTarget.get(), renderTarget->type(), 0, 0, 0, 0, 0, 0.0f });

				renderTargets = Brainfryer::RenderTargets::Create(renderTargetsInfo);
			}
			if (!renderTargets->initialized())
				return 10;

			std::unique_ptr<Brainfryer::DescriptorHeap> globalDescriptorHeap;
			std::vector<Brainfryer::DescriptorHeapRef>  renderTargetDescriptorRefs;
			Brainfryer::DescriptorHeapRef               imageDescriptorRef;
			{
				Brainfryer::DescriptorHeapInfo info {};
				info.type            = Brainfryer::EDescriptorHeapType::SRV_CBV_UAV;
				info.capacity        = renderTarget->imageCount() + 1;
				info.shaderVisible   = true;
				globalDescriptorHeap = Brainfryer::DescriptorHeap::Create(info);
				if (!globalDescriptorHeap->initialized())
					return 11;

				Brainfryer::FrameImageView renderTargetView {};
				renderTargetView.image     = renderTarget.get();
				renderTargetView.type      = renderTarget->type();
				renderTargetView.mipLevels = 1;
				renderTargetDescriptorRefs = globalDescriptorHeap->createFrameImageViews(renderTargetView);

				Brainfryer::ImageView view {};
				view.image         = image.get();
				view.type          = image->type();
				view.mipLevels     = 1;
				imageDescriptorRef = globalDescriptorHeap->createImageView(view);
			}

			loadCommandList->end();

			Brainfryer::Context::ExecuteCommandLists({ loadCommandList.get() });

			Brainfryer::Context::WaitForGPU();

			while (!window->requestedClose())
			{
				auto          commandList = Brainfryer::Context::NextFrame();
				std::uint32_t frameIndex  = Brainfryer::Context::FrameIndex();

				commandList->begin();
				commandList->begin(scenePipeline.get());

				commandList->setDescriptorHeaps({ globalDescriptorHeap.get() });

				renderTarget->transition(commandList, frameIndex, Brainfryer::ImageState::RenderTarget);
				commandList->bindRenderTargets(renderTargets.get(), frameIndex);
				commandList->setViewports({ Brainfryer::Viewport { 0, 0, static_cast<float>(renderTarget->width(frameIndex)), static_cast<float>(renderTarget->height(frameIndex)), 0.0f, 1.0f } });
				commandList->setScissors({ Brainfryer::Rect { 0, 0, renderTarget->width(frameIndex), renderTarget->height(frameIndex) } });

				commandList->bindDescriptorTable(0, imageDescriptorRef);

				commandList->setPrimitiveTopology(Brainfryer::EPrimitiveTopology::Triangles);
				commandList->setVertexBuffers(0, { vertexBufferView });
				commandList->setIndexBuffer(indexBufferView);
				commandList->drawIndexedInstanced(6, 1, 0, 0, 0);

				blitPipeline->bind(commandList);

				renderTarget->transition(commandList, frameIndex, Brainfryer::ImageState::PixelShaderResource);
				swapchain->bind(commandList);
				swapchain->clear(commandList, 0.1f, 0.1f, 0.1f, 1.0f);

				commandList->bindDescriptorTable(0, renderTargetDescriptorRefs[frameIndex]);
				commandList->setPrimitiveTopology(Brainfryer::EPrimitiveTopology::Triangles);
				commandList->setVertexBuffers(0, { blitVertexBufferView });
				commandList->drawInstanced(6, 1, 0, 0);

				swapchain->unbind(commandList);

				commandList->end();

				Brainfryer::Context::ExecuteCommandLists({ commandList });

				swapchain->present();

				Brainfryer::Window::MsgLoop();
			}

			Brainfryer::Context::WaitForGPU();
		}
		Brainfryer::Context::Destroy();
		return 0;*/
	}
} // namespace Brainfryer::Editor

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	Brainfryer::Utils::HookThrow();
	return static_cast<int>(Brainfryer::SafeExecute(&Brainfryer::Editor::SafeMain, argc, argv));
} // namespace Brainfryer::Editor

#if BUILD_IS_SYSTEM_WINDOWS

#include <Brainfryer/Envs/Windows/Win32.h>

int WinMain([[maybe_unused]] Brainfryer::Windows::HINSTANCE hInstance,
            [[maybe_unused]] Brainfryer::Windows::HINSTANCE hPrevInstance,
            [[maybe_unused]] Brainfryer::Windows::LPSTR     lpszCmdLine,
            [[maybe_unused]] int                            nCmdShow)
{
	return main(__argc, __argv);
}

#endif