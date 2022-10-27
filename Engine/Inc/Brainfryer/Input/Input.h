#pragma once

#include "Brainfryer/Utils/UID.h"
#include "ButtonStates.h"
#include "Inputs.h"

namespace Brainfryer::Input
{
	inline InputGroup* GetGroup(UID id) { return Inputs::Get().getGroup(id); }
	inline InputGroup* CreateGroup(UID id) { return Inputs::Get().createGroup(id); }
	inline InputGroup* GetOrCreateGroup(UID id) { return Inputs::Get().getOrCreateGroup(id); }

	inline void EnableGroup(UID id) { Inputs::Get().enableGroup(id); }
	inline void DisableGroup(UID id) { Inputs::Get().disableGroup(id); }
	inline void ToggleGroup(UID id) { Inputs::Get().toggleGroup(id); }
	inline bool IsGroupEnabled(UID id) { return Inputs::Get().isGroupEnabled(id); }

	inline bool         Button(UID id);
	inline bool         Button(Binding button);
	inline bool         Button(UID group, UID id);
	inline std::uint8_t ButtonState(UID id);
	inline std::uint8_t ButtonState(Binding button);
	inline std::uint8_t ButtonState(UID group, UID id);
	inline bool         ButtonPressed(UID id);
	inline bool         ButtonPressed(Binding button);
	inline bool         ButtonPressed(UID group, UID id);
	inline bool         ButtonRepeated(UID id);
	inline bool         ButtonRepeated(Binding button);
	inline bool         ButtonRepeated(UID group, UID id);
	inline bool         ButtonReleased(UID id);
	inline bool         ButtonReleased(Binding button);
	inline bool         ButtonReleased(UID group, UID id);

	inline float      Axis(UID id);
	inline float      Axis(Binding axis);
	inline float      Axis(UID group, UID id);
	inline glm::fvec2 Axis2D(UID id);
	inline glm::fvec2 Axis2D(Binding xAxis, Binding yAxis);
	inline glm::fvec2 Axis2D(UID group, UID id);
	inline glm::fvec3 Axis3D(UID id);
	inline glm::fvec3 Axis3D(Binding xAxis, Binding yAxis, Binding zAxis);
	inline glm::fvec3 Axis3D(UID group, UID id);

	template <Bindable T>
	inline void RegisterBinding(UID id, T&& binding);
	template <Bindable T>
	inline void RegisterBinding(UID group, UID id, T&& binding);
	inline void RegisterButtonBinding(UID id, Binding button);
	inline void RegisterButtonBinding(UID group, UID id, Binding button);
	inline void RegisterAxisBinding(UID id, Binding axis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterAxisBinding(UID group, UID id, Binding axis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterAxis2DBinding(UID id, Binding xAxis, Binding yAxis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterAxis2DBinding(UID group, UID id, Binding xAxis, Binding yAxis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterAxis3DBinding(UID id, Binding xAxis, Binding yAxis, Binding zAxis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterAxis3DBinding(UID group, UID id, Binding xAxis, Binding yAxis, Binding zAxis, EAxisMode mode = EAxisMode::Direct);
	inline void RegisterButtonAxisBinding(UID id, Binding posButton, Binding negButton);
	inline void RegisterButtonAxisBinding(UID group, UID id, Binding posButton, Binding negButton);
	inline void RegisterButtonAxis2DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton);
	inline void RegisterButtonAxis2DBinding(UID group, UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton);
	inline void RegisterButtonAxis3DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton, Binding posZButton, Binding negZButton);
	inline void RegisterButtonAxis3DBinding(UID group, UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton, Binding posZButton, Binding negZButton);
} // namespace Brainfryer::Input

namespace Brainfryer::Input
{
	inline bool         Button(UID id) { return Inputs::Get().getButton(id); }
	inline bool         Button(Binding button) { return Inputs::Get().getButtonState(button) & ButtonStates::DownMask; }
	inline bool         Button(UID group, UID id) { return GetGroup(group)->getButton(id); }
	inline std::uint8_t ButtonState(UID id) { return Inputs::Get().InputGroup::getButtonState(id); }
	inline std::uint8_t ButtonState(Binding button) { return Inputs::Get().getButtonState(button); }
	inline std::uint8_t ButtonState(UID group, UID id) { return GetGroup(group)->InputGroup::getButtonState(id); }
	inline bool         ButtonPressed(UID id) { return ButtonState(id) & ButtonStates::PressedMask; }
	inline bool         ButtonPressed(Binding button) { return ButtonState(button) & ButtonStates::PressedMask; }
	inline bool         ButtonPressed(UID group, UID id) { return ButtonState(group, id) & ButtonStates::PressedMask; }
	inline bool         ButtonRepeated(UID id) { return ButtonState(id) & ButtonStates::RepeatedMask; }
	inline bool         ButtonRepeated(Binding button) { return ButtonState(button) & ButtonStates::RepeatedMask; }
	inline bool         ButtonRepeated(UID group, UID id) { return ButtonState(group, id) & ButtonStates::RepeatedMask; }
	inline bool         ButtonReleased(UID id) { return ButtonState(id) & ButtonStates::ReleasedMask; }
	inline bool         ButtonReleased(Binding button) { return ButtonState(button) & ButtonStates::ReleasedMask; }
	inline bool         ButtonReleased(UID group, UID id) { return ButtonState(group, id) & ButtonStates::ReleasedMask; }

	inline float      Axis(UID id) { return Inputs::Get().getAxis(id); }
	inline float      Axis(Binding axis) { return Inputs::Get().getAxisState(axis); }
	inline float      Axis(UID group, UID id) { return GetGroup(group)->getAxis(id); }
	inline glm::fvec2 Axis2D(UID id) { return Inputs::Get().getAxis2D(id); }
	inline glm::fvec2 Axis2D(Binding xAxis, Binding yAxis) { return glm::fvec2 { Axis(xAxis), Axis(yAxis) }; }
	inline glm::fvec2 Axis2D(UID group, UID id) { return GetGroup(group)->getAxis2D(id); }
	inline glm::fvec3 Axis3D(UID id) { return Inputs::Get().getAxis3D(id); }
	inline glm::fvec3 Axis3D(Binding xAxis, Binding yAxis, Binding zAxis) { return glm::fvec3 { Axis(xAxis), Axis(yAxis), Axis(zAxis) }; }
	inline glm::fvec3 Axis3D(UID group, UID id) { return GetGroup(group)->getAxis3D(id); }

	template <Bindable T>
	inline void RegisterBinding(UID id, T&& binding)
	{
		Inputs::Get().registerBinding(id, std::forward<T>(binding));
	}

	template <Bindable T>
	inline void RegisterBinding(UID group, UID id, T&& binding)
	{
		GetGroup(group)->registerBinding(id, std::forward<T>(binding));
	}

	inline void RegisterButtonBinding(UID id, Binding button)
	{
		RegisterBinding(id, ButtonBinding { id, button });
	}

	inline void RegisterButtonBinding(UID group, UID id, Binding button)
	{
		RegisterBinding(group, id, ButtonBinding { id, button });
	}

	inline void RegisterAxisBinding(UID id, Binding axis, EAxisMode mode)
	{
		RegisterBinding(id, AxisBinding { id, axis, mode });
	}

	inline void RegisterAxisBinding(UID group, UID id, Binding axis, EAxisMode mode)
	{
		RegisterBinding(group, id, AxisBinding { id, axis, mode });
	}

	inline void RegisterAxis2DBinding(UID id, Binding xAxis, Binding yAxis, EAxisMode mode)
	{
		RegisterBinding(id, Axis2DBinding { id, xAxis, yAxis, mode });
	}

	inline void RegisterAxis2DBinding(UID group, UID id, Binding xAxis, Binding yAxis, EAxisMode mode)
	{
		RegisterBinding(group, id, Axis2DBinding { id, xAxis, yAxis, mode });
	}

	inline void RegisterAxis3DBinding(UID id, Binding xAxis, Binding yAxis, Binding zAxis, EAxisMode mode)
	{
		RegisterBinding(id, Axis3DBinding { id, xAxis, yAxis, zAxis, mode });
	}

	inline void RegisterAxis3DBinding(UID group, UID id, Binding xAxis, Binding yAxis, Binding zAxis, EAxisMode mode)
	{
		RegisterBinding(group, id, Axis3DBinding { id, xAxis, yAxis, zAxis, mode });
	}

	inline void RegisterButtonAxisBinding(UID id, Binding posButton, Binding negButton)
	{
		RegisterBinding(id, ButtonAxisBinding { id, posButton, negButton });
	}

	inline void RegisterButtonAxisBinding(UID group, UID id, Binding posButton, Binding negButton)
	{
		RegisterBinding(group, id, ButtonAxisBinding { id, posButton, negButton });
	}

	inline void RegisterButtonAxis2DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton)
	{
		RegisterBinding(id, ButtonAxis2DBinding { id, posXButton, negXButton, posYButton, negYButton });
	}

	inline void RegisterButtonAxis2DBinding(UID group, UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton)
	{
		RegisterBinding(group, id, ButtonAxis2DBinding { id, posXButton, negXButton, posYButton, negYButton });
	}

	inline void RegisterButtonAxis3DBinding(UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton, Binding posZButton, Binding negZButton)
	{
		RegisterBinding(id, ButtonAxis3DBinding { id, posXButton, negXButton, posYButton, negYButton, posZButton, negZButton });
	}

	inline void RegisterButtonAxis3DBinding(UID group, UID id, Binding posXButton, Binding negXButton, Binding posYButton, Binding negYButton, Binding posZButton, Binding negZButton)
	{
		RegisterBinding(group, id, ButtonAxis3DBinding { id, posXButton, negXButton, posYButton, negYButton, posZButton, negZButton });
	}
} // namespace Brainfryer::Input