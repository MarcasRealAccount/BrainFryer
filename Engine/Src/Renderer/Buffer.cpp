#include "Brainfryer/Renderer/Buffer.h"
#include "Brainfryer/Envs/DX12/DX12Buffer.h"
#include "Brainfryer/Renderer/Context.h"
#include "Brainfryer/Utils/Core.h"

namespace Brainfryer
{
	std::unique_ptr<Buffer> Buffer::Create(const BufferInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12Buffer>(info);
			else
				return nullptr;
		}
		return nullptr;
	}

	std::unique_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferInfo& info)
	{
		switch (Context::CurrentAPI())
		{
		case EContextAPI::None: return nullptr;
		case EContextAPI::DX12:
			if constexpr (Core::s_IsSystemWindows)
				return std::make_unique<DX12::DX12FrameBuffer>(info);
			else
				return nullptr;
		}
		return nullptr;
	}
} // namespace Brainfryer