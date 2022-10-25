#pragma once

#include "BufferView.h"
#include "DescriptorHeapRef.h"
#include "PrimitiveTopology.h"

#include <vector>

namespace Brainfryer
{
	class GraphicsPipeline;

	enum class ECommandListType
	{
		Direct,
		Bundle,
		Compute,
		Copy,
		VideoDecode,
		VideoProcess,
		VideoEncode
	};

	class CommandList
	{
	public:
		virtual ~CommandList() = default;

		virtual void begin(GraphicsPipeline* initialPipeline = nullptr) = 0;
		virtual void end()                                              = 0;

		virtual void setDescriptorHeaps(const std::vector<DescriptorHeap*>& heaps)         = 0;
		virtual void bindDescriptorTable(std::uint32_t binding, DescriptorHeapRef heapRef) = 0;

		virtual void setPrimitiveTopology(EPrimitiveTopology topology)                                        = 0;
		virtual void setVertexBuffers(std::uint32_t startIndex, const std::vector<VertexBufferView>& buffers) = 0;
		virtual void setIndexBuffer(IndexBufferView indexBuffer)                                              = 0;

		virtual void drawInstanced(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t startVertex, std::uint32_t startInstance)                                 = 0;
		virtual void drawIndexedInstanced(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t startIndex, std::uint32_t startVertex, std::uint32_t startInstance) = 0;

		virtual ECommandListType type() const = 0;
	};
} // namespace Brainfryer