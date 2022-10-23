#pragma once

#include <memory>

namespace Brainfryer
{
	class CommandList;

	enum class EHeapType
	{
		Default,
		Upload
	};

	enum class EBufferState
	{
		Common            = 0x000,
		VertexAndConstant = 0x001,
		Index             = 0x002,
		UnorderedAccess   = 0x008,
		StreamOut         = 0x100,
		IndirectArgument  = 0x200,
		CopyDst           = 0x400,
		CopySrc           = 0x800
	};

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

		virtual void* map()   = 0;
		virtual void  unmap() = 0;

		virtual void transition(CommandList* commandList, EBufferState state) = 0;

		virtual EHeapType    heapType() const = 0;
		virtual EBufferState state() const    = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer