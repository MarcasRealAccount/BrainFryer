#include "Brainfryer/Input/Devices/Mouse.h"
#include "Brainfryer/Input/ButtonStates.h"

#include <cstddef>

namespace Brainfryer::Input::Devices
{
	void Mouse::update()
	{
		// Reset the wheel axes to 0.0f
		m_Axes[AxisIndices::MouseWheelX] = 0.0f;
		m_Axes[AxisIndices::MouseWheelY] = 0.0f;

		for (std::size_t i = 0; i < c_MaxButtons; ++i)
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

	void Mouse::setAxis(std::uint32_t axis, float value)
	{
		if (axis < 4)
			m_Axes[axis] = value;
	}

	void Mouse::buttonPressed(std::uint32_t button)
	{
		if (button < c_MaxButtons)
			m_Buttons[button] |= ButtonStates::PressedMask | ButtonStates::DownMask;
	}

	void Mouse::buttonRepeated(std::uint32_t button)
	{
		if (button < c_MaxButtons)
			m_Buttons[button] |= ButtonStates::RepeatedMask | ButtonStates::DownMask;
	}

	void Mouse::buttonReleased(std::uint32_t button)
	{
		if (button < c_MaxButtons)
			m_Buttons[button] |= ButtonStates::ReleasedMask;
	}

	float Mouse::getAxis(std::uint32_t axis) const
	{
		return axis < 4 ? m_Axes[axis] : 0.0f;
	}

	std::uint8_t Mouse::getState(std::uint32_t button) const
	{
		return button < c_MaxButtons ? m_Buttons[button] : 0;
	}

	bool Mouse::isButtonPressed(std::uint32_t button) const
	{
		return button < c_MaxButtons ? m_Buttons[button] & ButtonStates::PressedMask : false;
	}

	bool Mouse::isButtonRepeated(std::uint32_t button) const
	{
		return button < c_MaxButtons ? m_Buttons[button] & ButtonStates::RepeatedMask : false;
	}

	bool Mouse::isButtonReleased(std::uint32_t button) const
	{
		return button < c_MaxButtons ? m_Buttons[button] & ButtonStates::ReleasedMask : false;
	}

	bool Mouse::isButtonDown(std::uint32_t button) const
	{
		return button < c_MaxButtons ? m_Buttons[button] & ButtonStates::DownMask : false;
	}
} // namespace Brainfryer::Input::Devices