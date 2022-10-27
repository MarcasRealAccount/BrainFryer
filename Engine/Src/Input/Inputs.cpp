#include "Brainfryer/Input/Inputs.h"

namespace Brainfryer::Input
{
	Inputs& Inputs::Get()
	{
		static Inputs* s_Instance = new Inputs();
		return *s_Instance;
	}

	void Inputs::Destroy()
	{
		delete &Get();
	}

	void Inputs::updateJoysticks(std::vector<Devices::Joystick>&& joysticks)
	{
		m_Joysticks = std::move(joysticks);
	}

	void Inputs::update()
	{
		updateGroup();
		for (auto& [name, group] : m_Groups)
			group.updateGroup();

		m_Keyboard.update();
		m_Mouse.update();
		for (auto& joystick : m_Joysticks)
			joystick.update();
	}

	void Inputs::postUpdate()
	{
		postUpdateGroup();
		for (auto& [name, group] : m_Groups)
			group.postUpdateGroup();
	}

	void Inputs::keyPressed(std::uint32_t key)
	{
		m_Keyboard.keyPressed(key);
		setButton(Binding { EInputLocation::Keyboard, 0, key }, m_Keyboard.getState(key));
	}

	void Inputs::keyRepeated(std::uint32_t key)
	{
		m_Keyboard.keyRepeated(key);
		setButton(Binding { EInputLocation::Keyboard, 0, key }, m_Keyboard.getState(key));
	}

	void Inputs::keyReleased(std::uint32_t key)
	{
		m_Keyboard.keyReleased(key);
		setButton(Binding { EInputLocation::Keyboard, 0, key }, m_Keyboard.getState(key));
	}

	void Inputs::mouseMove(float x, float y)
	{
		m_Mouse.setAxis(AxisIndices::MouseX, x);
		m_Mouse.setAxis(AxisIndices::MouseY, y);
		setAxis(Axes::MouseX(), x);
		setAxis(Axes::MouseY(), y);
	}

	void Inputs::mouseScroll(float x, float y)
	{
		m_Mouse.setAxis(AxisIndices::MouseWheelX, x);
		m_Mouse.setAxis(AxisIndices::MouseWheelY, y);
		setAxis(Axes::MouseWheelX(), x);
		setAxis(Axes::MouseWheelY(), y);
	}

	void Inputs::mouseButtonPressed(std::uint32_t button)
	{
		m_Mouse.buttonPressed(button);
		setButton(Binding { EInputLocation::Mouse, 0, button }, m_Mouse.getState(button));
	}

	void Inputs::mouseButtonRepeated(std::uint32_t button)
	{
		m_Mouse.buttonRepeated(button);
		setButton(Binding { EInputLocation::Mouse, 0, button }, m_Mouse.getState(button));
	}

	void Inputs::mouseButtonReleased(std::uint32_t button)
	{
		m_Mouse.buttonReleased(button);
		setButton(Binding { EInputLocation::Mouse, 0, button }, m_Mouse.getState(button));
	}

	void Inputs::joystickConnect(std::uint32_t index, std::uint32_t deviceID, std::uint32_t axes, std::uint32_t buttons, bool gamepad)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.setDeviceID(deviceID);
		if (gamepad)
			joystick.setGamepad();
		joystick.resizeAxises(axes);
		joystick.resizeButtons(buttons);
	}

	void Inputs::joystickDisconnect(std::uint32_t index)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.setDeviceID(0);
		joystick.resizeAxises(0);
		joystick.resizeButtons(0);
	}

	void Inputs::joystickAxis(std::uint32_t index, std::uint32_t deviceID, std::uint32_t axis, float value)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.setAxis(axis, value);
		setAxis(Binding { joystick.isGamepad() ? EInputLocation::Gamepad : EInputLocation::Joystick, deviceID, axis }, value);
	}

	void Inputs::joystickButtonPressed(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.buttonPressed(button);
		setButton(Binding { joystick.isGamepad() ? EInputLocation::Gamepad : EInputLocation::Joystick, deviceID, button }, joystick.getState(button));
	}

	void Inputs::joystickButtonRepeated(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.buttonRepeated(button);
		setButton(Binding { joystick.isGamepad() ? EInputLocation::Gamepad : EInputLocation::Joystick, deviceID, button }, joystick.getState(button));
	}

	void Inputs::joystickButtonReleased(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button)
	{
		if (index >= m_Joysticks.size())
			return;

		auto& joystick = m_Joysticks[index];
		joystick.buttonReleased(button);
		setButton(Binding { joystick.isGamepad() ? EInputLocation::Gamepad : EInputLocation::Joystick, deviceID, button }, joystick.getState(button));
	}

	InputGroup* Inputs::getGroup(std::uint32_t id)
	{
		if (id == 0)
			return this;

		auto itr = m_Groups.find(id);
		return itr != m_Groups.end() ? &itr->second : nullptr;
	}

	const InputGroup* Inputs::getGroup(std::uint32_t id) const
	{
		if (id == 0)
			return this;

		auto itr = m_Groups.find(id);
		return itr != m_Groups.end() ? &itr->second : nullptr;
	}

	InputGroup* Inputs::createGroup(std::uint32_t id)
	{
		if (id == 0)
			return nullptr;

		auto itr = m_Groups.insert({ id, InputGroup { this, id } });
		return itr.second ? &itr.first->second : nullptr;
	}

	InputGroup* Inputs::getOrCreateGroup(std::uint32_t id)
	{
		auto group = getGroup(id);
		return group ? group : createGroup(id);
	}

	void Inputs::enableGroup(std::uint32_t id)
	{
		auto group = getGroup(id);
		if (group)
			group->enable();
	}

	void Inputs::disableGroup(std::uint32_t id)
	{
		auto group = getGroup(id);
		if (group)
			group->disable();
	}

	void Inputs::toggleGroup(std::uint32_t id)
	{
		auto group = getGroup(id);
		if (group)
			group->toggle();
	}

	bool Inputs::isGroupEnabled(std::uint32_t id) const
	{
		auto group = getGroup(id);
		return group ? group->isEnabled() : false;
	}

	float Inputs::getAxisState(Binding binding) const
	{
		switch (binding.m_Location)
		{
		case EInputLocation::Mouse: return m_Mouse.getAxis(binding.m_Index);
		case EInputLocation::Gamepad: [[fallthrough]];
		case EInputLocation::Joystick: return getJoystick(binding.m_DeviceID)->getAxis(binding.m_Index);
		default: return 0.0f;
		}
	}

	std::uint8_t Inputs::getButtonState(Binding binding) const
	{
		switch (binding.m_Location)
		{
		case EInputLocation::Keyboard: return m_Keyboard.getState(binding.m_Index);
		case EInputLocation::Mouse: return m_Mouse.getState(binding.m_Index);
		case EInputLocation::Gamepad: [[fallthrough]];
		case EInputLocation::Joystick: return getJoystick(binding.m_DeviceID)->getState(binding.m_Index);
		default: return 0;
		}
	}

	Devices::Joystick* Inputs::getJoystick(std::uint32_t deviceID)
	{
		for (auto& joystick : m_Joysticks)
			if (joystick.getDeviceID() == deviceID)
				return &joystick;
		return nullptr;
	}

	const Devices::Joystick* Inputs::getJoystick(std::uint32_t deviceID) const
	{
		for (auto& joystick : m_Joysticks)
			if (joystick.getDeviceID() == deviceID)
				return &joystick;
		return nullptr;
	}

	Inputs::Inputs()
	    : InputGroup(this, 0) {}

	void Inputs::setAxis(Binding binding, float value)
	{
		setAxisGroup(binding, value);
		for (auto& [name, group] : m_Groups)
			if (group.isEnabled())
				group.setAxisGroup(binding, value);
	}

	void Inputs::setButton(Binding binding, std::uint8_t state)
	{
		setButtonGroup(binding, state);
		for (auto& [name, group] : m_Groups)
			if (group.isEnabled())
				group.setButtonGroup(binding, state);
	}
} // namespace Brainfryer::Input