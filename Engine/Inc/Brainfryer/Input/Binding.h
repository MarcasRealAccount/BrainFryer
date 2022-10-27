#pragma once

#include "Brainfryer/Utils/UID.h"

#include <cstdint>

#include <glm/glm.hpp>

namespace Brainfryer::Input
{
	enum class EBindingType
	{
		Button,
		Axis,
		Axis2D,
		Axis3D,
		ButtonAxis,
		ButtonAxis2D,
		ButtonAxis3D
	};

	enum class EInputLocation
	{
		Keyboard,
		Mouse,
		Gamepad,
		Joystick
	};

	enum class EAxisMode
	{
		Direct,
		Relative
	};

	struct Binding
	{
	public:
		constexpr Binding(EInputLocation location, std::uint32_t deviceID, std::uint32_t index, float sensitivity = 1.0f)
		    : m_Location(location), m_DeviceID(deviceID), m_Index(index), m_Sensitivity(sensitivity) {}

		friend bool operator==(Binding lhs, Binding rhs);
		friend bool operator!=(Binding lhs, Binding rhs) { return !(lhs == rhs); }

	public:
		EInputLocation m_Location;
		std::uint32_t  m_DeviceID;
		std::uint32_t  m_Index;
		float          m_Sensitivity;
	};

	struct ButtonBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::Button;

	public:
		ButtonBinding(UID id, Binding button)
		    : m_ID(id), m_Button(button), m_State(0) {}

	public:
		UID     m_ID;
		Binding m_Button;

		std::uint8_t m_State;
	};

	struct AxisBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::Axis;

	public:
		AxisBinding(UID id, Binding axis, EAxisMode mode)
		    : m_ID(id), m_Axis(axis), m_Mode(mode), m_State(0.0f), m_PState(0.0f) {}

	public:
		UID       m_ID;
		Binding   m_Axis;
		EAxisMode m_Mode;

		float m_State;
		float m_PState;
	};

	struct Axis2DBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::Axis2D;

	public:
		Axis2DBinding(UID id, Binding xAxis, Binding yAxis, EAxisMode mode)
		    : m_ID(id), m_XAxis(xAxis), m_YAxis(yAxis), m_Mode(mode), m_State({ 0.0f, 0.0f }), m_PState({ 0.0f, 0.0f }) {}

	public:
		UID       m_ID;
		Binding   m_XAxis;
		Binding   m_YAxis;
		EAxisMode m_Mode;

		glm::fvec2 m_State;
		glm::fvec2 m_PState;
	};

	struct Axis3DBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::Axis3D;

	public:
		Axis3DBinding(UID id, Binding xAxis, Binding yAxis, Binding zAxis, EAxisMode mode)
		    : m_ID(id), m_XAxis(xAxis), m_YAxis(yAxis), m_ZAxis(zAxis), m_Mode(mode), m_State({ 0.0f, 0.0f, 0.0f }), m_PState({ 0.0f, 0.0f, 0.0f }) {}

	public:
		UID       m_ID;
		Binding   m_XAxis;
		Binding   m_YAxis;
		Binding   m_ZAxis;
		EAxisMode m_Mode;

		glm::fvec3 m_State  = { 0.0f, 0.0f, 0.0f };
		glm::fvec3 m_PState = { 0.0f, 0.0f, 0.0f };
	};

	struct ButtonAxisBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::ButtonAxis;

	public:
		ButtonAxisBinding(UID id, Binding posButton, Binding negButton)
		    : m_ID(id), m_PosButton(posButton), m_NegButton(negButton), m_State(0.0f) {}

	public:
		UID     m_ID;
		Binding m_PosButton;
		Binding m_NegButton;

		float m_State = 0.0f;
	};

	struct ButtonAxis2DBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::ButtonAxis2D;

	public:
		ButtonAxis2DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton)
		    : m_ID(id), m_PosXButton(posXButton), m_NegXButton(negXButton), m_PosYButton(posYButton), m_NegYButton(negYButton), m_State({ 0.0f, 0.0f }) {}

	public:
		UID     m_ID;
		Binding m_PosXButton;
		Binding m_NegXButton;
		Binding m_PosYButton;
		Binding m_NegYButton;

		glm::fvec2 m_State;
	};

	struct ButtonAxis3DBinding
	{
	public:
		static constexpr EBindingType Type = EBindingType::ButtonAxis3D;

	public:
		ButtonAxis3DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton, Binding posZButton, Binding negZButton)
		    : m_ID(id), m_PosXButton(posXButton), m_NegXButton(negXButton), m_PosYButton(posYButton), m_NegYButton(negYButton), m_PosZButton(posZButton), m_NegZButton(negZButton), m_State({ 0.0f, 0.0f, 0.0f }) {}

	public:
		UID     m_ID;
		Binding m_PosXButton;
		Binding m_NegXButton;
		Binding m_PosYButton;
		Binding m_NegYButton;
		Binding m_PosZButton;
		Binding m_NegZButton;

		glm::fvec3 m_State = { 0.0f, 0.0f, 0.0f };
	};

	struct BindingInfo
	{
	public:
		BindingInfo(EBindingType type, std::size_t index)
		    : m_Type(type), m_Index(index) {}

	public:
		EBindingType m_Type;
		std::size_t  m_Index;
	};

	namespace Detail
	{
		template <EBindingType T>
		struct BindingType
		{
			using Type = void;
		};
		template <>
		struct BindingType<EBindingType::Button>
		{
			using Type = ButtonBinding;
		};
		template <>
		struct BindingType<EBindingType::Axis>
		{
			using Type = AxisBinding;
		};
		template <>
		struct BindingType<EBindingType::Axis2D>
		{
			using Type = Axis2DBinding;
		};
		template <>
		struct BindingType<EBindingType::Axis3D>
		{
			using Type = Axis3DBinding;
		};
		template <>
		struct BindingType<EBindingType::ButtonAxis>
		{
			using Type = ButtonAxisBinding;
		};
		template <>
		struct BindingType<EBindingType::ButtonAxis2D>
		{
			using Type = ButtonAxis2DBinding;
		};
		template <>
		struct BindingType<EBindingType::ButtonAxis3D>
		{
			using Type = ButtonAxis3DBinding;
		};
	} // namespace Detail

	template <EBindingType T>
	using BindingType = Detail::BindingType<T>::Type;

	template <class T>
	concept Bindable = std::is_same_v<std::remove_cv_t<decltype(T::Type)>, EBindingType>;
} // namespace Brainfryer::Input

#include "Devices/Joystick.h"
#include "Devices/Keyboard.h"
#include "Devices/Mouse.h"

namespace Brainfryer::Input
{
	namespace Buttons
	{
		constexpr Binding KeySpace(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeySpace, sensitivity }; }
		constexpr Binding KeyApostrophe(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyApostrophe, sensitivity }; }
		constexpr Binding KeyComma(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyComma, sensitivity }; }
		constexpr Binding KeyMinus(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyMinus, sensitivity }; }
		constexpr Binding KeyPeriod(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyPeriod, sensitivity }; }
		constexpr Binding KeySlash(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeySlash, sensitivity }; }
		constexpr Binding Key0(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key0, sensitivity }; }
		constexpr Binding Key1(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key1, sensitivity }; }
		constexpr Binding Key2(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key2, sensitivity }; }
		constexpr Binding Key3(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key3, sensitivity }; }
		constexpr Binding Key4(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key4, sensitivity }; }
		constexpr Binding Key5(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key5, sensitivity }; }
		constexpr Binding Key6(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key6, sensitivity }; }
		constexpr Binding Key7(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key7, sensitivity }; }
		constexpr Binding Key8(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key8, sensitivity }; }
		constexpr Binding Key9(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::Key9, sensitivity }; }
		constexpr Binding KeySemicolon(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeySemicolon, sensitivity }; }
		constexpr Binding KeyEqual(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyEqual, sensitivity }; }
		constexpr Binding KeyA(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyA, sensitivity }; }
		constexpr Binding KeyB(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyB, sensitivity }; }
		constexpr Binding KeyC(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyC, sensitivity }; }
		constexpr Binding KeyD(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyD, sensitivity }; }
		constexpr Binding KeyE(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyE, sensitivity }; }
		constexpr Binding KeyF(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF, sensitivity }; }
		constexpr Binding KeyG(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyG, sensitivity }; }
		constexpr Binding KeyH(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyH, sensitivity }; }
		constexpr Binding KeyI(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyI, sensitivity }; }
		constexpr Binding KeyJ(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyJ, sensitivity }; }
		constexpr Binding KeyK(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyK, sensitivity }; }
		constexpr Binding KeyL(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyL, sensitivity }; }
		constexpr Binding KeyM(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyM, sensitivity }; }
		constexpr Binding KeyN(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyN, sensitivity }; }
		constexpr Binding KeyO(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyO, sensitivity }; }
		constexpr Binding KeyP(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyP, sensitivity }; }
		constexpr Binding KeyQ(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyQ, sensitivity }; }
		constexpr Binding KeyR(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyR, sensitivity }; }
		constexpr Binding KeyS(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyS, sensitivity }; }
		constexpr Binding KeyT(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyT, sensitivity }; }
		constexpr Binding KeyU(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyU, sensitivity }; }
		constexpr Binding KeyV(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyV, sensitivity }; }
		constexpr Binding KeyW(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyW, sensitivity }; }
		constexpr Binding KeyX(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyX, sensitivity }; }
		constexpr Binding KeyY(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyY, sensitivity }; }
		constexpr Binding KeyZ(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyZ, sensitivity }; }
		constexpr Binding KeyLeftBracket(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeftBracket, sensitivity }; }
		constexpr Binding KeyBackslash(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyBackslash, sensitivity }; }
		constexpr Binding KeyRightBracket(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyRightBracket, sensitivity }; }
		constexpr Binding KeyGraveAccent(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyGraveAccent, sensitivity }; }
		constexpr Binding KeyWorld1(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyWorld1, sensitivity }; }
		constexpr Binding KeyWorld2(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyWorld2, sensitivity }; }
		constexpr Binding KeyEscape(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyEscape, sensitivity }; }
		constexpr Binding KeyEnter(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyEnter, sensitivity }; }
		constexpr Binding KeyTab(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyTab, sensitivity }; }
		constexpr Binding KeyBackspace(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyBackspace, sensitivity }; }
		constexpr Binding KeyInsert(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyInsert, sensitivity }; }
		constexpr Binding KeyDelete(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyDelete, sensitivity }; }
		constexpr Binding KeyRight(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyRight, sensitivity }; }
		constexpr Binding KeyLeft(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeft, sensitivity }; }
		constexpr Binding KeyDown(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyDown, sensitivity }; }
		constexpr Binding KeyUp(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyUp, sensitivity }; }
		constexpr Binding KeyPageUp(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyPageUp, sensitivity }; }
		constexpr Binding KeyPageDown(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyPageDown, sensitivity }; }
		constexpr Binding KeyHome(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyHome, sensitivity }; }
		constexpr Binding KeyEnd(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyEnd, sensitivity }; }
		constexpr Binding KeyCapsLock(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyCapsLock, sensitivity }; }
		constexpr Binding KeyScrollLock(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyScrollLock, sensitivity }; }
		constexpr Binding KeyNumLock(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyNumLock, sensitivity }; }
		constexpr Binding KeyPrintScreen(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyPrintScreen, sensitivity }; }
		constexpr Binding KeyPause(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyPause, sensitivity }; }
		constexpr Binding KeyF1(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF1, sensitivity }; }
		constexpr Binding KeyF2(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF2, sensitivity }; }
		constexpr Binding KeyF3(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF3, sensitivity }; }
		constexpr Binding KeyF4(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF4, sensitivity }; }
		constexpr Binding KeyF5(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF5, sensitivity }; }
		constexpr Binding KeyF6(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF6, sensitivity }; }
		constexpr Binding KeyF7(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF7, sensitivity }; }
		constexpr Binding KeyF8(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF8, sensitivity }; }
		constexpr Binding KeyF9(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF9, sensitivity }; }
		constexpr Binding KeyF10(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF10, sensitivity }; }
		constexpr Binding KeyF11(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF11, sensitivity }; }
		constexpr Binding KeyF12(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF12, sensitivity }; }
		constexpr Binding KeyF13(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF13, sensitivity }; }
		constexpr Binding KeyF14(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF14, sensitivity }; }
		constexpr Binding KeyF15(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF15, sensitivity }; }
		constexpr Binding KeyF16(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF16, sensitivity }; }
		constexpr Binding KeyF17(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF17, sensitivity }; }
		constexpr Binding KeyF18(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF18, sensitivity }; }
		constexpr Binding KeyF19(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF19, sensitivity }; }
		constexpr Binding KeyF20(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF20, sensitivity }; }
		constexpr Binding KeyF21(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF21, sensitivity }; }
		constexpr Binding KeyF22(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF22, sensitivity }; }
		constexpr Binding KeyF23(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF23, sensitivity }; }
		constexpr Binding KeyF24(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF24, sensitivity }; }
		constexpr Binding KeyF25(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyF25, sensitivity }; }
		constexpr Binding KeyKp0(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp0, sensitivity }; }
		constexpr Binding KeyKp1(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp1, sensitivity }; }
		constexpr Binding KeyKp2(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp2, sensitivity }; }
		constexpr Binding KeyKp3(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp3, sensitivity }; }
		constexpr Binding KeyKp4(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp4, sensitivity }; }
		constexpr Binding KeyKp5(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp5, sensitivity }; }
		constexpr Binding KeyKp6(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp6, sensitivity }; }
		constexpr Binding KeyKp7(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp7, sensitivity }; }
		constexpr Binding KeyKp8(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp8, sensitivity }; }
		constexpr Binding KeyKp9(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKp9, sensitivity }; }
		constexpr Binding KeyKpDecimal(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpDecimal, sensitivity }; }
		constexpr Binding KeyKpDivide(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpDivide, sensitivity }; }
		constexpr Binding KeyKpMultiply(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpMultiply, sensitivity }; }
		constexpr Binding KeyKpSubtract(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpSubtract, sensitivity }; }
		constexpr Binding KeyKpAdd(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpAdd, sensitivity }; }
		constexpr Binding KeyKpEnter(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpEnter, sensitivity }; }
		constexpr Binding KeyKpEqual(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyKpEqual, sensitivity }; }
		constexpr Binding KeyLeftShift(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeftShift, sensitivity }; }
		constexpr Binding KeyLeftControl(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeftControl, sensitivity }; }
		constexpr Binding KeyLeftAlt(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeftAlt, sensitivity }; }
		constexpr Binding KeyLeftSuper(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyLeftSuper, sensitivity }; }
		constexpr Binding KeyRightShift(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyRightShift, sensitivity }; }
		constexpr Binding KeyRightControl(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, Input::ButtonIndices::KeyRightControl, sensitivity }; }
		constexpr Binding KeyRightAlt(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyRightAlt, sensitivity }; }
		constexpr Binding KeyRightSuper(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyRightSuper, sensitivity }; }
		constexpr Binding KeyMenu(float sensitivity = 1.0f) { return Binding { EInputLocation::Keyboard, 0, ButtonIndices::KeyMenu, sensitivity }; }

		constexpr Binding Mouse1(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse1, sensitivity }; }
		constexpr Binding Mouse2(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse2, sensitivity }; }
		constexpr Binding Mouse3(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse3, sensitivity }; }
		constexpr Binding Mouse4(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse4, sensitivity }; }
		constexpr Binding Mouse5(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse5, sensitivity }; }
		constexpr Binding Mouse6(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse6, sensitivity }; }
		constexpr Binding Mouse7(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse7, sensitivity }; }
		constexpr Binding Mouse8(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, ButtonIndices::Mouse8, sensitivity }; }
		constexpr Binding MouseLeft(float sensitivity = 1.0f) { return Mouse1(sensitivity); }
		constexpr Binding MouseRight(float sensitivity = 1.0f) { return Mouse2(sensitivity); }
		constexpr Binding MouseMiddle(float sensitivity = 1.0f) { return Mouse3(sensitivity); }

		constexpr Binding GamepadA(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadA, sensitivity }; }
		constexpr Binding GamepadB(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadB, sensitivity }; }
		constexpr Binding GamepadX(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadX, sensitivity }; }
		constexpr Binding GamepadY(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadY, sensitivity }; }
		constexpr Binding GamepadLeftBumper(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadLeftBumper, sensitivity }; }
		constexpr Binding GamepadRightBumper(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadRightBumper, sensitivity }; }
		constexpr Binding GamepadBack(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadBack, sensitivity }; }
		constexpr Binding GamepadStart(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadStart, sensitivity }; }
		constexpr Binding GamepadGuide(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadGuide, sensitivity }; }
		constexpr Binding GamepadLeftThumb(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadLeftThumb, sensitivity }; }
		constexpr Binding GamepadRightThumb(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadRightThumb, sensitivity }; }
		constexpr Binding GamepadDPadUp(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadDPadUp, sensitivity }; }
		constexpr Binding GamepadDPadRight(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadDPadRight, sensitivity }; }
		constexpr Binding GamepadDPadDown(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadDPadDown, sensitivity }; }
		constexpr Binding GamepadDPadLeft(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, ButtonIndices::GamepadDPadLeft, sensitivity }; }
		constexpr Binding GamepadCross(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return GamepadA(deviceID, sensitivity); }
		constexpr Binding GamepadCircle(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return GamepadB(deviceID, sensitivity); }
		constexpr Binding GamepadSquare(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return GamepadX(deviceID, sensitivity); }
		constexpr Binding GamepadTriangle(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return GamepadY(deviceID, sensitivity); }
	} // namespace Buttons

	namespace Axes
	{
		constexpr Binding MouseX(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, AxisIndices::MouseX, sensitivity }; }
		constexpr Binding MouseY(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, AxisIndices::MouseY, sensitivity }; }
		constexpr Binding MouseWheelX(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, AxisIndices::MouseWheelX, sensitivity }; }
		constexpr Binding MouseWheelY(float sensitivity = 1.0f) { return Binding { EInputLocation::Mouse, 0, AxisIndices::MouseWheelY, sensitivity }; }

		constexpr Binding GamepadLeftX(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadLeftX, sensitivity }; }
		constexpr Binding GamepadLeftY(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadLeftY, sensitivity }; }
		constexpr Binding GamepadRightX(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadRightX, sensitivity }; }
		constexpr Binding GamepadRightY(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadRightY, sensitivity }; }
		constexpr Binding GamepadLeftTrigger(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadLeftTrigger, sensitivity }; }
		constexpr Binding GamepadRightTrigger(std::uint32_t deviceID = 0, float sensitivity = 1.0f) { return Binding { EInputLocation::Gamepad, deviceID, AxisIndices::GamepadRightTrigger, sensitivity }; }
	} // namespace Axes
} // namespace Brainfryer::Input