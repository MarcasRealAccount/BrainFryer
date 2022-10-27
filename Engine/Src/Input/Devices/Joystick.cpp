#include "Brainfryer/Input/Devices/Joystick.h"
#include "Brainfryer/Input/ButtonStates.h"

namespace Brainfryer::Input::Devices
{
	void Joystick::update()
	{
		if (m_DeviceID == 0)
			return;

		for (std::size_t i = 0; i < m_Buttons.size(); ++i)
		{
			auto& button = m_Buttons[i];
			// If button is released set the state to 0, I.e. not pressed, repeated, released or down
			// otherwise set the state to be down if it already is down otherwise 0
			if (button & ButtonStates::ReleasedMask)
				button = 0;
			else
				button &= ButtonStates::DownMask;
		}
	}

	void Joystick::resizeAxises(std::uint32_t axises)
	{
		m_Axes.resize(axises, 0.0f);
	}

	void Joystick::resizeButtons(std::uint32_t buttons)
	{
		m_Buttons.resize(buttons, 0);
	}

	void Joystick::setAxis(std::uint32_t axis, float value)
	{
		if (axis < m_Axes.size())
			m_Axes[axis] = value;
	}

	void Joystick::buttonPressed(std::uint32_t button)
	{
		if (button < m_Buttons.size())
			m_Buttons[button] |= ButtonStates::PressedMask | ButtonStates::DownMask;
	}

	void Joystick::buttonRepeated(std::uint32_t button)
	{
		if (button < m_Buttons.size())
			m_Buttons[button] |= ButtonStates::RepeatedMask | ButtonStates::DownMask;
	}

	void Joystick::buttonReleased(std::uint32_t button)
	{
		if (button < m_Buttons.size())
			m_Buttons[button] |= ButtonStates::ReleasedMask;
	}

	float Joystick::getAxis(std::uint32_t axis) const
	{
		return axis < m_Axes.size() ? m_Axes[axis] : 0.0f;
	}

	std::uint8_t Joystick::getState(std::uint32_t button) const
	{
		return button < m_Buttons.size() ? m_Buttons[button] : 0;
	}

	bool Joystick::isButtonPressed(std::uint32_t button) const
	{
		return button < m_Buttons.size() ? m_Buttons[button] & ButtonStates::PressedMask : false;
	}

	bool Joystick::isButtonRepeated(std::uint32_t button) const
	{
		return button < m_Buttons.size() ? m_Buttons[button] & ButtonStates::RepeatedMask : false;
	}

	bool Joystick::isButtonReleased(std::uint32_t button) const
	{
		return button < m_Buttons.size() ? m_Buttons[button] & ButtonStates::ReleasedMask : false;
	}

	bool Joystick::isButtonDown(std::uint32_t button) const
	{
		return button < m_Buttons.size() ? m_Buttons[button] & ButtonStates::DownMask : false;
	}
} // namespace Brainfryer::Input::Devices