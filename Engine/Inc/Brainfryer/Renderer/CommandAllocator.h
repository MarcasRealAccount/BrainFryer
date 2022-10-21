#pragma once

#include "CommandList.h"

#include <memory>
#include <vector>

namespace Brainfryer
{
	struct CommandAllocatorInfo
	{
	public:
		ECommandListType type;
	};

	class CommandAllocator
	{
	public:
		static std::unique_ptr<CommandAllocator> Create(const CommandAllocatorInfo& info);

	public:
		virtual ~CommandAllocator() = default;

		virtual void reset() = 0;

		virtual std::vector<std::unique_ptr<CommandList>> allocate(std::size_t numCommandLists) = 0;

		virtual ECommandListType type() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer