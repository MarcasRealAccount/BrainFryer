#include "Envs/DX12/DXCompiler.h"
#include "Utils/Core.h"
#include <Brainfryer/Envs/DX12/DX12Context.h>
#include <Brainfryer/Renderer/Buffer.h>
#include <Brainfryer/Renderer/CommandAllocator.h>
#include <Brainfryer/Renderer/CommandList.h>
#include <Brainfryer/Renderer/Context.h>
#include <Brainfryer/Renderer/GraphicsPipeline.h>
#include <Brainfryer/Renderer/PipelineLayout.h>
#include <Brainfryer/Renderer/Swapchain.h>
#include <Brainfryer/Utils/Exception.h>
#include <Brainfryer/Utils/Log.h>
#include <Brainfryer/Window/Window.h>

int safeMain()
{
	Brainfryer::WindowSpecification specs;
	specs.title = "BrainFryer editor";
	specs.state = Brainfryer::EWindowState::Maximized;

	std::unique_ptr<Brainfryer::Window> window = Brainfryer::Window::Create(specs);
	if (!window->initialized())
		return 1;

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

		std::unique_ptr<Brainfryer::PipelineLayout> graphicsPipelineLayout;
		{
			Brainfryer::PipelineLayoutInfo pipelineLayoutInfo {};
			pipelineLayoutInfo.flags = Brainfryer::EPipelineLayoutFlags::AllowInputAssemblerInputLayout;
			graphicsPipelineLayout   = Brainfryer::PipelineLayout::Create(std::move(pipelineLayoutInfo));
		}
		if (!graphicsPipelineLayout->initialized())
			return 4;

		std::unique_ptr<Brainfryer::GraphicsPipeline> graphicsPipeline;
		{
			Brainfryer::GraphicsPipelineInfo graphicsPipelineInfo {};
			graphicsPipelineInfo.pipelineLayout = graphicsPipelineLayout.get();

			DX12::Com<DX12::ID3D10Blob> vertexShader;
			DX12::Com<DX12::ID3D10Blob> pixelShader;

			std::uint32_t compileFlags = 0;
			if constexpr (Core::s_IsConfigDebug)
				compileFlags |= DX12::D3DCOMPILE_DEBUG | DX12::D3DCOMPILE_SKIP_OPTIMIZATION;

			DX12::D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, vertexShader, nullptr);
			DX12::D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, pixelShader, nullptr);

			graphicsPipelineInfo.vertexShader.data.resize(vertexShader->GetBufferSize());
			graphicsPipelineInfo.pixelShader.data.resize(pixelShader->GetBufferSize());
			std::memcpy(graphicsPipelineInfo.vertexShader.data.data(), vertexShader->GetBufferPointer(), vertexShader->GetBufferSize());
			std::memcpy(graphicsPipelineInfo.pixelShader.data.data(), pixelShader->GetBufferPointer(), pixelShader->GetBufferSize());

			graphicsPipelineInfo.inputs.emplace_back("POSITION", 0, Brainfryer::EFormat::R32G32B32_FLOAT, 0, 0, 0);
			graphicsPipelineInfo.inputs.emplace_back("COLOR", 0, Brainfryer::EFormat::R32G32B32A32_FLOAT, 0, 12, 0);

			graphicsPipelineInfo.primitiveTopology = Brainfryer::EPrimitiveTopology::Triangles;

			graphicsPipelineInfo.renderTargetCount = 1;
			graphicsPipelineInfo.rtvFormats[0]     = swapchain->format();

			graphicsPipeline = Brainfryer::GraphicsPipeline::Create(graphicsPipelineInfo);
		}
		if (!graphicsPipeline->initialized())
			return 5;

		std::unique_ptr<Brainfryer::Buffer> buffer;
		Brainfryer::VertexBufferView        vertexBufferView {};
		{
			float triangleVertices[] {
				0.0f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
				-0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
			};

			Brainfryer::BufferInfo bufferInfo {};
			bufferInfo.heapType  = Brainfryer::EHeapType::Upload;
			bufferInfo.alignment = 0;
			bufferInfo.size      = sizeof(triangleVertices);

			buffer = Brainfryer::Buffer::Create(bufferInfo);
			if (!buffer->initialized())
				return 6;

			void* memory = buffer->map();
			std::memcpy(memory, triangleVertices, sizeof(triangleVertices));
			buffer->unmap();

			vertexBufferView.buffer = buffer.get();
			vertexBufferView.offset = 0;
			vertexBufferView.size   = sizeof(triangleVertices);
			vertexBufferView.stride = 28;
		}

		Brainfryer::Context::WaitForGPU();

		while (!window->requestedClose())
		{
			auto commandList = Brainfryer::Context::NextFrame();

			commandList->begin(graphicsPipeline.get());

			swapchain->bind(commandList);
			swapchain->clear(commandList, 0.1f, 0.1f, 0.1f, 1.0f);

			commandList->setPrimitiveTopology(Brainfryer::EPrimitiveTopology::Triangles);
			commandList->setVertexBuffers(0, { vertexBufferView });
			commandList->drawInstanced(3, 1, 0, 0);

			swapchain->unbind(commandList);

			commandList->end();

			Brainfryer::Context::ExecuteCommandLists({ commandList });

			swapchain->present();

			window->msgLoop();
		}

		Brainfryer::Context::WaitForGPU();
	}
	Brainfryer::Context::Destroy();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	try
	{
		return safeMain();
	}
	catch (const Brainfryer::Utils::Exception& exception)
	{
		Brainfryer::Log::GetOrCreateLogger(exception.title())->critical("{}", exception);
		Brainfryer::Window::FatalErrorBox(exception.what(), exception.title(), exception.backTrace());
		return 0x7FFF'FFFF;
	}
	catch (const std::exception& exception)
	{
		Brainfryer::Log::Critical("{}", exception.what());
		Brainfryer::Window::FatalErrorBox(exception.what());
		return 0x0000'7FFF;
	}
}

#if BUILD_IS_SYSTEM_WINDOWS

#undef APIENTRY
#include <Windows.h>

int WinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance, [[maybe_unused]] _In_ LPSTR lpszCmdLine, [[maybe_unused]] _In_ int nCmdShow)
{
	return main(__argc, __argv);
}

#endif