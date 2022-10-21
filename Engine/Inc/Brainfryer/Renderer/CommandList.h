#pragma once

namespace Brainfryer
{
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

		virtual void begin() = 0;
		virtual void end()   = 0;

		virtual ECommandListType type() const = 0;
	};
} // namespace Brainfryer