#pragma once

#include <cstddef>
#include <cstdint>

#include <vector>

namespace Brainfryer::Input
{
	namespace Devices
	{
		class Joystick final
		{
		public:
			Joystick()  = default;
			~Joystick() = default;

			void setDeviceID(std::uint32_t deviceID) { m_DeviceID = deviceID; }
			void setGamepad() { m_IsGamepad = true; }

			void update();

			void resizeAxises(std::uint32_t axises);
			void resizeButtons(std::uint32_t buttons);

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

			auto getDeviceID() const { return m_DeviceID; }
			bool isConnected() const { return m_DeviceID != 0; }
			auto isGamepad() const { return m_IsGamepad; }
			auto getAxisCount() const { return m_Axes.size(); }
			auto getButtonCount() const { return m_Buttons.size(); }

		private:
			std::uint32_t m_DeviceID  = 0;
			bool          m_IsGamepad = false;

			std::vector<float>        m_Axes;
			std::vector<std::uint8_t> m_Buttons;
		};
	} // namespace Devices

	namespace ButtonIndices
	{
		static constexpr std::uint32_t GamepadA           = 0;
		static constexpr std::uint32_t GamepadB           = 1;
		static constexpr std::uint32_t GamepadX           = 2;
		static constexpr std::uint32_t GamepadY           = 3;
		static constexpr std::uint32_t GamepadLeftBumper  = 4;
		static constexpr std::uint32_t GamepadRightBumper = 5;
		static constexpr std::uint32_t GamepadBack        = 6;
		static constexpr std::uint32_t GamepadStart       = 7;
		static constexpr std::uint32_t GamepadGuide       = 8;
		static constexpr std::uint32_t GamepadLeftThumb   = 9;
		static constexpr std::uint32_t GamepadRightThumb  = 10;
		static constexpr std::uint32_t GamepadDPadUp      = 11;
		static constexpr std::uint32_t GamepadDPadRight   = 12;
		static constexpr std::uint32_t GamepadDPadDown    = 13;
		static constexpr std::uint32_t GamepadDPadLeft    = 14;
		static constexpr std::uint32_t GamepadCross       = GamepadA;
		static constexpr std::uint32_t GamepadCircle      = GamepadB;
		static constexpr std::uint32_t GamepadSquare      = GamepadX;
		static constexpr std::uint32_t GamepadTriangle    = GamepadY;
	} // namespace ButtonIndices

	namespace AxisIndices
	{
		static constexpr std::uint32_t GamepadLeftX        = 0;
		static constexpr std::uint32_t GamepadLeftY        = 1;
		static constexpr std::uint32_t GamepadRightX       = 2;
		static constexpr std::uint32_t GamepadRightY       = 3;
		static constexpr std::uint32_t GamepadLeftTrigger  = 4;
		static constexpr std::uint32_t GamepadRightTrigger = 5;
	} // namespace AxisIndices
} // namespace Brainfryer::Input