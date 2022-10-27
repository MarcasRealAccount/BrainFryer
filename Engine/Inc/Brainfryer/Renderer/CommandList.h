#pragma once

#include "Brainfryer/Utils/Rect.h"
#include "BufferView.h"
#include "DescriptorHeapRef.h"
#include "PrimitiveTopology.h"

#include <vector>

namespace Brainfryer
{
	class GraphicsPipeline;
	class RenderTargets;

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

	struct Viewport
	{
	public:
		float x, y, w, h;
		float minDepth, maxDepth;
	};

	class CommandList
	{
	public:
		virtual ~CommandList() = default;

		virtual void begin(GraphicsPipeline* initialPipeline = nullptr) = 0;
		virtual void end()                                              = 0;

		virtual void bindRenderTargets(RenderTargets* renderTargets, std::uint32_t index) = 0;
		virtual void setViewports(const std::vector<Viewport>& viewports)                 = 0;
		virtual void setScissors(const std::vector<Rect>& scissors)                       = 0;

		virtual void setDescriptorHeaps(const std::vector<DescriptorHeap*>& heaps)                                          = 0;
		virtual void bindDescriptorTable(std::uint32_t binding, DescriptorHeapRef heapRef)                                  = 0;
		virtual void bind32BitConstants(std::uint32_t binding, std::uint32_t count, const void* data, std::uint32_t offset) = 0;

		virtual void setBlendFactor(const float (&factor)[4])                                                 = 0;
		virtual void setPrimitiveTopology(EPrimitiveTopology topology)                                        = 0;
		virtual void setVertexBuffers(std::uint32_t startIndex, const std::vector<VertexBufferView>& buffers) = 0;
		virtual void setIndexBuffer(IndexBufferView indexBuffer)                                              = 0;

		virtual void drawInstanced(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t startVertex, std::uint32_t startInstance)                                 = 0;
		virtual void drawIndexedInstanced(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t startIndex, std::uint32_t startVertex, std::uint32_t startInstance) = 0;

		virtual ECommandListType type() const = 0;
	};
} // namespace Brainfryer