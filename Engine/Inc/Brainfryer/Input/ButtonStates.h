#pragma once

#include <cstdint>

namespace Brainfryer::Input
{
	namespace ButtonStates
	{
		static constexpr std::uint8_t PressedMask  = 0b0001;
		static constexpr std::uint8_t RepeatedMask = 0b0010;
		static constexpr std::uint8_t ReleasedMask = 0b0100;
		static constexpr std::uint8_t DownMask     = 0b1000;

		static constexpr std::uint8_t PressedBit  = 0;
		static constexpr std::uint8_t RepeatBit   = 1;
		static constexpr std::uint8_t ReleasedBit = 2;
		static constexpr std::uint8_t DownBit     = 3;
	} // namespace ButtonStates

	enum class EButtonState
	{
		Pressed,
		Repeated,
		Released,
		Down
	};
} // namespace Brainfryer::Input