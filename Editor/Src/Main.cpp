#include "Utils/Core.h"
#include <Brainfryer/Envs/DX12/DX12Context.h>
#include <Brainfryer/Renderer/CommandAllocator.h>
#include <Brainfryer/Renderer/CommandList.h>
#include <Brainfryer/Renderer/Context.h>
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

		while (!window->requestedClose())
		{
			auto commandList = Brainfryer::Context::NextFrame();

			commandList->begin();

			swapchain->bind(commandList);
			swapchain->clear(commandList, 0.1f, 0.1f, 0.1f, 1.0f);

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
		safeMain();
	}
	catch (const Brainfryer::Utils::Exception& exception)
	{
		Brainfryer::Log::GetOrCreateLogger(exception.title())->critical("{}", exception);
		Brainfryer::Window::FatalErrorBox(exception.what(), exception.title(), exception.backTrace());
	}
	catch (const std::exception& exception)
	{
		Brainfryer::Log::Critical("{}", exception.what());
		Brainfryer::Window::FatalErrorBox(exception.what());
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