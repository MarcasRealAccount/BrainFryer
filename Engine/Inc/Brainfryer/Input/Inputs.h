#pragma once

#include "Devices/Joystick.h"
#include "Devices/Keyboard.h"
#include "Devices/Mouse.h"
#include "InputGroup.h"

#include <unordered_map>

namespace Brainfryer::Input
{
	class Inputs final : public InputGroup
	{
	public:
		static Inputs& Get();
		static void    Destroy();

	public:
		void updateJoysticks(std::vector<Devices::Joystick>&& joysticks);

		void update();
		void postUpdate();

		void keyPressed(std::uint32_t key);
		void keyRepeated(std::uint32_t key);
		void keyReleased(std::uint32_t key);

		void mouseMove(float x, float y);
		void mouseScroll(float x, float y);

		void mouseButtonPressed(std::uint32_t button);
		void mouseButtonRepeated(std::uint32_t button);
		void mouseButtonReleased(std::uint32_t button);

		void joystickConnect(std::uint32_t index, std::uint32_t deviceID, std::uint32_t axes, std::uint32_t buttons, bool gamepad);
		void joystickDisconnect(std::uint32_t index);
		void joystickAxis(std::uint32_t index, std::uint32_t deviceID, std::uint32_t axis, float value);
		void joystickButtonPressed(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button);
		void joystickButtonRepeated(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button);
		void joystickButtonReleased(std::uint32_t index, std::uint32_t deviceID, std::uint32_t button);

		InputGroup*       getGroup(std::uint32_t id);
		const InputGroup* getGroup(std::uint32_t id) const;
		InputGroup*       createGroup(std::uint32_t id);
		InputGroup*       getOrCreateGroup(std::uint32_t id);

		void enableGroup(std::uint32_t id);
		void disableGroup(std::uint32_t id);
		void toggleGroup(std::uint32_t id);
		bool isGroupEnabled(std::uint32_t id) const;

		float        getAxisState(Binding binding) const;
		std::uint8_t getButtonState(Binding binding) const;

		auto&                    getKeyboard() const { return m_Keyboard; }
		auto&                    getMouse() const { return m_Mouse; }
		auto&                    getJoysticks() const { return m_Joysticks; }
		Devices::Joystick*       getJoystick(std::uint32_t deviceID);
		const Devices::Joystick* getJoystick(std::uint32_t deviceID) const;

	private:
		Inputs();
		~Inputs() = default;

		void setAxis(Binding binding, float value);
		void setButton(Binding binding, std::uint8_t state);

	private:
		Devices::Keyboard m_Keyboard;
		Devices::Mouse    m_Mouse;

		std::vector<Devices::Joystick> m_Joysticks;

		std::unordered_map<std::uint32_t, InputGroup> m_Groups;
	};
} // namespace Brainfryer::Input