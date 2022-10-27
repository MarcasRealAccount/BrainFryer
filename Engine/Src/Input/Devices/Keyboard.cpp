#include "Brainfryer/Input/Devices/Keyboard.h"
#include "Brainfryer/Input/ButtonStates.h"

#include <cstddef>

namespace Brainfryer::Input::Devices
{
	void Keyboard::update()
	{
		for (std::size_t i = 0; i < c_MaxKeys; ++i)
		{
			auto& key = m_Keys[i];
			// If key is released set the state to 0, I.e. not pressed, repeated, released or down
			// otherwise set the state to be down if it already is down otherwise 0
			if (key & ButtonStates::ReleasedMask)
				key = 0;
			else
				key &= ButtonStates::DownMask;
		}
	}

	void Keyboard::keyPressed(std::uint32_t key)
	{
		if (key < c_MaxKeys)
			m_Keys[key] |= ButtonStates::PressedMask | ButtonStates::DownMask;
	}

	void Keyboard::keyRepeated(std::uint32_t key)
	{
		if (key < c_MaxKeys)
			m_Keys[key] |= ButtonStates::RepeatedMask | ButtonStates::DownMask;
	}

	void Keyboard::keyReleased(std::uint32_t key)
	{
		if (key < c_MaxKeys)
			m_Keys[key] |= ButtonStates::ReleasedMask;
	}

	std::uint8_t Keyboard::getState(std::uint32_t key) const
	{
		return key < c_MaxKeys ? m_Keys[key] : 0;
	}

	bool Keyboard::isKeyPressed(std::uint32_t key) const
	{
		return key < c_MaxKeys ? m_Keys[key] & ButtonStates::PressedMask : false;
	}

	bool Keyboard::isKeyRepeated(std::uint32_t key) const
	{
		return key < c_MaxKeys ? m_Keys[key] & ButtonStates::RepeatedMask : false;
	}

	bool Keyboard::isKeyReleased(std::uint32_t key) const
	{
		return key < c_MaxKeys ? m_Keys[key] & ButtonStates::ReleasedMask : false;
	}

	bool Keyboard::isKeyDown(std::uint32_t key) const
	{
		return key < c_MaxKeys ? m_Keys[key] & ButtonStates::DownMask : false;
	}
} // namespace Brainfryer::Input::Devices