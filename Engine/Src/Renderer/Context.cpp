#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<Context> Context::s_CurrentContext = nullptr;

	bool Context::SelectSpecificAPI(EContextAPI api)
	{
		if (s_CurrentContext)
			return s_CurrentContext->api() == api;

		switch (api)
		{
		case EContextAPI::None:
			break;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				s_CurrentContext = std::make_unique<DX12::DX12Context>();
			break;
		}

		return s_CurrentContext && s_CurrentContext->initialized();
	}

	bool Context::SelectBestAPI()
	{
		if (s_CurrentContext)
			return true;

		if constexpr (Core::s_IsSystemWindows)
			s_CurrentContext = std::make_unique<DX12::DX12Context>();
		return s_CurrentContext && s_CurrentContext->initialized();
	}

	void Context::Destroy()
	{
		s_CurrentContext = nullptr;
	}

	void Context::ExecuteCommandLists(const std::vector<CommandList*>& commandLists)
	{
		if (s_CurrentContext)
			s_CurrentContext->executeCommandLists(commandLists);
	}

	void Context::WaitForGPU()
	{
		if (s_CurrentContext)
			s_CurrentContext->waitForGPU();
	}

	UID Context::NewCMDList()
	{
		if (s_CurrentContext)
			return s_CurrentContext->newCMDList();
		return {};
	}

	void Context::DestroyCMDList(UID id)
	{
		if (s_CurrentContext)
			s_CurrentContext->destroyCMDList(id);
	}

	CommandList* Context::CurrentCMDList(UID id)
	{
		if (s_CurrentContext)
			return s_CurrentContext->currentCMDList(id);
		return nullptr;
	}

	CommandList* Context::NextFrame()
	{
		if (s_CurrentContext)
			return s_CurrentContext->nextFrame();
		return nullptr;
	}

	std::uint32_t Context::FrameIndex()
	{
		if (s_CurrentContext)
			return s_CurrentContext->frameIndex();
		return 0;
	}

	std::uint32_t Context::FrameCount()
	{
		if (s_CurrentContext)
			return s_CurrentContext->frameCount();
		return 0;
	}

	EContextAPI Context::CurrentAPI()
	{
		return s_CurrentContext ? s_CurrentContext->api() : EContextAPI::None;
	}
} // namespace Brainfryer