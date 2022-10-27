#pragma once

#include <cstddef>
#include <cstdint>

namespace Brainfryer::Input
{
	namespace Devices
	{
		class Mouse final
		{
		public:
			static constexpr std::size_t c_MaxButtons = 8;

		public:
			Mouse()  = default;
			~Mouse() = default;

			void update();

			void setAxis(std::uint32_t axis, float value);

			void buttonPressed(std::uint32_t button);
			void buttonRepeated(std::uint32_t button);
			void buttonReleased(std::uint32_t button);

			float getAxis(std::uint32_t axis) const;

			std::uint8_t getState(std::uint32_t button) const;

			bool isButtonPressed(std::uint32_t button) const;
			bool isButtonRepeated(std::uint32_t button) const;
			bool isButtonReleased(std::uint32_t button) const;
			bool isButtonDown(std::uint32_t button) const;

		private:
			float        m_Axes[4] { 0.0f };
			std::uint8_t m_Buttons[c_MaxButtons] { 0 };
		};
	} // namespace Devices

	namespace ButtonIndices
	{
		static constexpr std::uint32_t Mouse1      = 0;
		static constexpr std::uint32_t Mouse2      = 1;
		static constexpr std::uint32_t Mouse3      = 2;
		static constexpr std::uint32_t Mouse4      = 3;
		static constexpr std::uint32_t Mouse5      = 4;
		static constexpr std::uint32_t Mouse6      = 5;
		static constexpr std::uint32_t Mouse7      = 6;
		static constexpr std::uint32_t Mouse8      = 7;
		static constexpr std::uint32_t MouseLeft   = Mouse1;
		static constexpr std::uint32_t MouseRight  = Mouse2;
		static constexpr std::uint32_t MouseMiddle = Mouse3;
	} // namespace ButtonIndices

	namespace AxisIndices
	{
		static constexpr std::uint32_t MouseX      = 0;
		static constexpr std::uint32_t MouseY      = 1;
		static constexpr std::uint32_t MouseWheelX = 2;
		static constexpr std::uint32_t MouseWheelY = 3;
	} // namespace AxisIndices
} // namespace Brainfryer::Input