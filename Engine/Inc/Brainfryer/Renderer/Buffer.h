#pragma once

#include "Brainfryer/Utils/Flags.h"
#include "BufferView.h"

#include <memory>

namespace Brainfryer
{
	class CommandList;

	enum class EHeapType
	{
		Default,
		Upload
	};

	using EBufferState = Utils::Flags<>;

	namespace BufferState
	{
		static constexpr EBufferState Common                 = 0x000;
		static constexpr EBufferState VertexAndConstant      = 0x001;
		static constexpr EBufferState Index                  = 0x002;
		static constexpr EBufferState UnorderedAccess        = 0x008;
		static constexpr EBufferState NonPixelShaderResource = 0x040;
		static constexpr EBufferState IndirectArgument       = 0x200;
		static constexpr EBufferState CopyDst                = 0x400;
		static constexpr EBufferState CopySrc                = 0x800;
	}; // namespace BufferState

	struct BufferInfo
	{
	public:
		EHeapType     heapType;
		EBufferState  initialState; // Unused if heapType == EHeapType.Upload;
		std::uint64_t alignment;
		std::uint64_t size;
	};

	class Buffer
	{
	public:
		static std::unique_ptr<Buffer> Create(const BufferInfo& info);

	public:
		virtual ~Buffer() = default;

		virtual void* map(std::uint64_t readStart = 0, std::uint64_t readSize = 0)                                      = 0;
		virtual void  unmap(std::uint64_t writeStart = 0, std::uint64_t writeSize = 0, bool explicitWriteRange = false) = 0;

		virtual void copyFrom(CommandList* commandList, BufferView view, std::uint64_t offset = 0) = 0;

		virtual void transition(CommandList* commandList, EBufferState state) = 0;

		virtual EHeapType    heapType() const = 0;
		virtual EBufferState state() const    = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer