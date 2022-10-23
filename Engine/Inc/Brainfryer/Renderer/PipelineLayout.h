#pragma once

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
		EPipelineLayoutDescriptorRangeType type;
		std::uint32_t                      numDescriptors;
		std::uint32_t                      binding;
		std::uint32_t                      space;
		std::uint32_t                      offset;
	};

	struct PipelineLayoutDescriptorTable
	{
		std::vector<PipelineLayoutDescriptorRange> ranges;
	};

	struct PipelineLayoutParameter
	{
	public:
		EPipelineLayoutParameterType type;
		EShaderVisibility            visiblity;
		std::variant<PipelineLayoutDescriptor,
		             PipelineLayoutConstants,
		             PipelineLayoutDescriptorTable>
		    value;
	};

	struct PipelineLayoutStaticSampler
	{
	public:
		// TODO(MarcasRealAccount): Implement static samplers
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