#pragma once

#include "ComparisonFunc.h"
#include "Filter.h"
#include "ImageAddressMode.h"

#include <memory>
#include <variant>
#include <vector>

namespace Brainfryer
{
	enum class EPipelineLayoutFlags
	{
		None                           = 0x0,
		AllowInputAssemblerInputLayout = 0x1
	};

	enum class EPipelineLayoutParameterType
	{
		DescriptorTable,
		Constants,
		ConstantBuffer,
		ShaderResourceView,
		UniformView
	};

	enum class EPipelineLayoutDescriptorRangeType
	{
		ShaderResourceView,
		UniformView,
		ConstantBuffer,
		Sampler
	};

	enum class EPipelineLayoutDescriptorRangeFlags
	{
		None                = 0x0,
		DescriptorsVolatile = 0x1,
		DataVolatile        = 0x2,
		DataStatic          = 0x8
	};

	enum class EShaderVisibility
	{
		All,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Amplification,
		Mesh
	};

	enum class EBorderColor
	{
		TransparentBlack,
		OpaqueBlack,
		OpaqueWhite
	};

	struct PipelineLayoutDescriptor
	{
		std::uint32_t binding;
		std::uint32_t space;
	};

	struct PipelineLayoutConstants
	{
		std::uint32_t binding;
		std::uint32_t space;
		std::uint32_t num32BitValues;
	};

	struct PipelineLayoutDescriptorRange
	{
		EPipelineLayoutDescriptorRangeType  type;
		std::uint32_t                       numDescriptors;
		std::uint32_t                       binding;
		std::uint32_t                       space;
		std::uint32_t                       offset;
		EPipelineLayoutDescriptorRangeFlags flags;
	};

	struct PipelineLayoutDescriptorTable
	{
		std::vector<PipelineLayoutDescriptorRange> ranges;
	};

	struct PipelineLayoutParameter
	{
	public:
		PipelineLayoutParameter() = default;
		PipelineLayoutParameter(EPipelineLayoutParameterType type, PipelineLayoutDescriptor descriptor, EShaderVisibility visibility = EShaderVisibility::All);
		PipelineLayoutParameter(PipelineLayoutConstants constants, EShaderVisibility visibility = EShaderVisibility::All);
		PipelineLayoutParameter(PipelineLayoutDescriptorTable descriptorTable, EShaderVisibility visibility = EShaderVisibility::All);

	public:
		EPipelineLayoutParameterType type;
		EShaderVisibility            visibility;
		std::variant<PipelineLayoutDescriptor,
		             PipelineLayoutConstants,
		             PipelineLayoutDescriptorTable>
		    value;
	};

	struct PipelineLayoutStaticSampler
	{
	public:
		EFilter           minFilter;
		EFilter           magFilter;
		EImageAddressMode addressU;
		EImageAddressMode addressV;
		EImageAddressMode addressW;
		float             mipLodBias;
		std::uint32_t     maxAnisotropy;
		EComparisonFunc   comparisonFunc;
		EBorderColor      borderColor;
		float             minLod;
		float             maxLod;
		std::uint32_t     binding;
		std::uint32_t     space;
		EShaderVisibility visibility;
	};

	struct PipelineLayoutInfo
	{
	public:
		std::vector<PipelineLayoutParameter>     parameters;
		std::vector<PipelineLayoutStaticSampler> staticSamplers;
		EPipelineLayoutFlags                     flags;
	};

	class PipelineLayout
	{
	public:
		static std::unique_ptr<PipelineLayout> Create(PipelineLayoutInfo info);

	public:
		virtual ~PipelineLayout() = default;

		virtual const PipelineLayoutInfo& info() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer