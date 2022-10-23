#pragma once

#include "Brainfryer/Renderer/GraphicsPipeline.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	class DX12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		DX12GraphicsPipeline(const GraphicsPipelineInfo& info);
		~DX12GraphicsPipeline();

		virtual void bind(CommandList* commandList) override;

		virtual PipelineLayout* pipelineLayout() const { return m_PipelineLayout; }

		virtual bool initialized() const override { return m_Pipeline.valid(); }

		auto& handle() { return m_Pipeline; }
		auto& handle() const { return m_Pipeline; }

		Com<ID3D12RootSignature>& rootSignature();
		Com<ID3D12RootSignature>& rootSignature() const;

	private:
		PipelineLayout* m_PipelineLayout;

		Com<ID3D12PipelineState> m_Pipeline;
	};
} // namespace Brainfryer::DX12