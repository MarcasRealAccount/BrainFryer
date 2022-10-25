#pragma once

#include "Brainfryer/Utils/Flags.h"
#include "ComparisonFunc.h"
#include "Filter.h"
#include "ImageAddressMode.h"

#include <memory>
#include <variant>
#include <vector>

namespace Brainfryer
{
	using EPipelineLayoutFlags                = Utils::Flags<>;
	using EPipelineLayoutDescriptorRangeFlags = Utils::Flags<>;

	namespace PipelineLayoutFlags
	{
		static constexpr EPipelineLayoutFlags None                           = 0x0;
		static constexpr EPipelineLayoutFlags AllowInputAssemblerInputLayout = 0x1;
	}; // namespace PipelineLayoutFlags

	namespace PipelineLayoutDescriptorRangeFlags
	{
		static constexpr EPipelineLayoutDescriptorRangeFlags None                = 0x0;
		static constexpr EPipelineLayoutDescriptorRangeFlags DescriptorsVolatile = 0x1;
		static constexpr EPipelineLayoutDescriptorRangeFlags DataVolatile        = 0x2;
		static constexpr EPipelineLayoutDescriptorRangeFlags DataStatic          = 0x8;
	}; // namespace PipelineLayoutDescriptorRangeFlags

	enum class EPipelineLayoutParameterType
	{
		DescriptorTable,
		Constants,
		ConstantBuffer,
		ShaderResourceView,
		UnorderedAccess
	};

	enum class EPipelineLayoutDescriptorRangeType
	{
		ShaderResourceView,
		UnorderedAccess,
		ConstantBuffer,
		Sampler
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