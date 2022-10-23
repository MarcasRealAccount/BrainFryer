#pragma once

#include "Format.h"
#include "PrimitiveTopology.h"

#include <memory>
#include <string>
#include <vector>

namespace Brainfryer
{
	class CommandList;
	class PipelineLayout;

	struct CompiledShaderSource
	{
	public:
		std::vector<std::uint8_t> data;
	};

	struct GraphicsPipelineInput
	{
	public:
		std::string   name;
		std::uint32_t index;
		EFormat       format;
		std::uint32_t binding;
		std::uint32_t offset;
		std::uint32_t instanceStep;
	};

	struct GraphicsPipelineInfo
	{
	public:
		PipelineLayout* pipelineLayout;

		CompiledShaderSource vertexShader;
		CompiledShaderSource pixelShader;
		CompiledShaderSource domainShader;
		CompiledShaderSource hullShader;
		CompiledShaderSource geometryShader;

		std::vector<GraphicsPipelineInput> inputs;

		EPrimitiveTopology primitiveTopology;

		std::uint8_t renderTargetCount;
		EFormat      rtvFormats[8];
		EFormat      dsvFormat;
	};

	class GraphicsPipeline
	{
	public:
		static std::unique_ptr<GraphicsPipeline> Create(const GraphicsPipelineInfo& info);

	public:
		virtual ~GraphicsPipeline() = default;

		virtual void bind(CommandList* commandList) = 0;

		virtual PipelineLayout* pipelineLayout() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer