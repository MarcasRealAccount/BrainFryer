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

	enum class EBlend
	{
		Zero = 1,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DstAlpha,
		InvDstAlpha,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
		Src1Color,
		InvSrc1Color,
		Src1Alpha,
		InvSrc1Alpha,
		AlphaFactor,
		InvAlphaFactor
	};

	enum class EBlendOp
	{
		Add = 1,
		Subtract,
		RevSubtract,
		Min,
		Max
	};

	enum class ELogicOp
	{
		Clear        = 0,
		Set          = (Clear + 1),
		Copy         = (Set + 1),
		CopyInverted = (Copy + 1),
		Noop         = (CopyInverted + 1),
		Invert       = (Noop + 1),
		And          = (Invert + 1),
		Nand         = (And + 1),
		Or           = (Nand + 1),
		Nor          = (Or + 1),
		Xor          = (Nor + 1),
		Equiv        = (Xor + 1),
		AndReverse   = (Equiv + 1),
		AndInverted  = (AndReverse + 1),
		OrReverse    = (AndInverted + 1),
		OrInverted   = (OrReverse + 1)
	};

	enum class EWindingOrder
	{
		CW,
		CCW
	};

	struct GraphicsPipelineBlendInfo
	{
	public:
		bool         blendEnable;
		bool         logicOpEnable;
		EBlend       srcBlend;
		EBlend       dstBlend;
		EBlendOp     blendOp;
		EBlend       srcBlendAlpha;
		EBlend       dstBlendAlpha;
		EBlendOp     blendOpAlpha;
		ELogicOp     logicOp;
		std::uint8_t writeMask;
	};

	struct GraphicsPipelineBlendState
	{
	public:
		bool                      alphaCoverageEnable;
		bool                      independentBlendEnable;
		GraphicsPipelineBlendInfo renderTarget[8];
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

		GraphicsPipelineBlendState blend;

		EPrimitiveTopology primitiveTopology;
		EWindingOrder      windingOrder;

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