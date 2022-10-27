#include "Brainfryer/Input/InputGroup.h"
#include "Brainfryer/Input/ButtonStates.h"
#include "Brainfryer/Input/Inputs.h"

namespace Brainfryer::Input
{
	InputGroup::InputGroup(Inputs* inputs, std::uint32_t id)
	    : m_Inputs(inputs), m_ID(id), m_Enabled(true) {}

	void InputGroup::enable()
	{
		m_Enabled = true;
	}

	void InputGroup::disable()
	{
		m_Enabled = false;
	}

	void InputGroup::toggle()
	{
		m_Enabled = !m_Enabled;
	}

	bool InputGroup::getButton(UID id) const
	{
		auto binding = getBinding<ButtonBinding>(id);
		return binding ? binding->m_State & ButtonStates::DownMask : false;
	}

	std::uint8_t InputGroup::getButtonState(UID id) const
	{
		auto binding = getBinding<ButtonBinding>(id);
		return binding ? binding->m_State : 0;
	}

	float InputGroup::getAxis(UID id) const
	{
		auto itr = m_BindingsMap.find(id);
		if (itr == m_BindingsMap.end())
			return 0.0f;

		auto info = itr->second;

		switch (info.m_Type)
		{
		case EBindingType::Axis:
		{
			// If the binding is an Axis we return either the direct or relative state of it
			auto binding = info.m_Index < m_AxisBindings.size() ? &m_AxisBindings[info.m_Index] : nullptr;
			if (!binding)
				return 0.0f;

			switch (binding->m_Mode)
			{
			case EAxisMode::Direct: return binding->m_State;
			case EAxisMode::Relative: return (binding->m_State - binding->m_PState) * binding->m_Axis.m_Sensitivity;
			default: return 0.0f;
			}
		}
		case EBindingType::ButtonAxis:
		{
			// If the binding is a ButtonAxis we just return the state
			auto binding = info.m_Index < m_ButtonAxisBindings.size() ? &m_ButtonAxisBindings[info.m_Index] : nullptr;
			if (!binding)
				return 0.0f;

			return binding->m_State;
		}
		default:
			return 0.0f;
		}
	}

	glm::fvec2 InputGroup::getAxis2D(UID id) const
	{
		auto itr = m_BindingsMap.find(id);
		if (itr == m_BindingsMap.end())
			return { 0.0f, 0.0f };

		auto info = itr->second;

		switch (info.m_Type)
		{
		case EBindingType::Axis2D:
		{
			// If the binding is an Axis2D we return either the direct or relative state of it
			auto binding = info.m_Index < m_Axis2DBindings.size() ? &m_Axis2DBindings[info.m_Index] : nullptr;
			if (!binding)
				return { 0.0f, 0.0f };

			switch (binding->m_Mode)
			{
			case EAxisMode::Direct: return binding->m_State;
			case EAxisMode::Relative: return (binding->m_State - binding->m_PState) * glm::fvec2 { binding->m_XAxis.m_Sensitivity, binding->m_YAxis.m_Sensitivity };
			default: return { 0.0f, 0.0f };
			}
		}
		case EBindingType::ButtonAxis2D:
		{
			// If the binding is a ButtonAxis2D we just return the state
			auto binding = info.m_Index < m_ButtonAxis2DBindings.size() ? &m_ButtonAxis2DBindings[info.m_Index] : nullptr;
			if (!binding)
				return { 0.0f, 0.0f };

			return binding->m_State;
		}
		default:
			return { 0.0f, 0.0f };
		}
	}

	glm::fvec3 InputGroup::getAxis3D(UID id) const
	{
		auto itr = m_BindingsMap.find(id);
		if (itr == m_BindingsMap.end())
			return { 0.0f, 0.0f, 0.0f };

		auto info = itr->second;

		switch (info.m_Type)
		{
		case EBindingType::Axis3D:
		{
			// If the binding is an Axis3D we return either the direct or relative state of it
			auto binding = info.m_Index < m_Axis3DBindings.size() ? &m_Axis3DBindings[info.m_Index] : nullptr;
			if (!binding)
				return { 0.0f, 0.0f, 0.0f };

			switch (binding->m_Mode)
			{
			case EAxisMode::Direct: return binding->m_State;
			case EAxisMode::Relative: return (binding->m_State - binding->m_PState) * glm::fvec3 { binding->m_XAxis.m_Sensitivity, binding->m_YAxis.m_Sensitivity, binding->m_ZAxis.m_Sensitivity };
			default: return { 0.0f, 0.0f, 0.0f };
			}
		}
		case EBindingType::ButtonAxis3D:
		{
			// If the binding is a ButtonAxis3D we just return the state
			auto binding = info.m_Index < m_ButtonAxis3DBindings.size() ? &m_ButtonAxis3DBindings[info.m_Index] : nullptr;
			if (!binding)
				return { 0.0f, 0.0f, 0.0f };

			return binding->m_State;
		}
		default:
			return { 0.0f, 0.0f, 0.0f };
		}
	}

	void InputGroup::updateGroup()
	{
		for (auto& binding : m_AxisBindings)
			binding.m_PState = binding.m_State;
		for (auto& binding : m_Axis2DBindings)
			binding.m_PState = binding.m_State;
		for (auto& binding : m_Axis3DBindings)
			binding.m_PState = binding.m_State;
	}

	void InputGroup::postUpdateGroup()
	{
		if (m_Enabled)
		{
			for (auto& binding : m_ButtonBindings)
				binding.m_State = m_Inputs->getButtonState(binding.m_Button);

			for (auto& binding : m_ButtonAxisBindings)
				binding.m_State = getButtonAxis(binding.m_PosButton, binding.m_NegButton);
			for (auto& binding : m_ButtonAxis2DBindings)
				binding.m_State = {
					getButtonAxis(binding.m_PosXButton, binding.m_NegXButton),
					getButtonAxis(binding.m_PosYButton, binding.m_NegYButton)
				};
			for (auto& binding : m_ButtonAxis3DBindings)
				binding.m_State = {
					getButtonAxis(binding.m_PosXButton, binding.m_NegXButton),
					getButtonAxis(binding.m_PosYButton, binding.m_NegYButton),
					getButtonAxis(binding.m_PosZButton, binding.m_NegZButton)
				};
		}
		else
		{
			// If the InputGroup is disabled we want all the button states to be 0
			for (auto& binding : m_ButtonBindings)
				binding.m_State = 0;
			for (auto& binding : m_ButtonAxisBindings)
				binding.m_State = 0.0f;
			for (auto& binding : m_ButtonAxis2DBindings)
				binding.m_State = { 0.0f, 0.0f };
			for (auto& binding : m_ButtonAxis3DBindings)
				binding.m_State = { 0.0f, 0.0f, 0.0f };
		}
	}

	void InputGroup::setAxisGroup(Binding binding, float value)
	{
		for (auto& bnd : m_AxisBindings)
			if (bnd.m_Axis == binding)
				bnd.m_State = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_Axis.m_Sensitivity : value;

		for (auto& bnd : m_Axis2DBindings)
		{
			if (bnd.m_XAxis == binding)
				bnd.m_State.x = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_XAxis.m_Sensitivity : value;
			if (bnd.m_YAxis == binding)
				bnd.m_State.y = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_YAxis.m_Sensitivity : value;
		}
		for (auto& bnd : m_Axis3DBindings)
		{
			if (bnd.m_XAxis == binding)
				bnd.m_State.x = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_XAxis.m_Sensitivity : value;
			if (bnd.m_YAxis == binding)
				bnd.m_State.y = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_YAxis.m_Sensitivity : value;
			if (bnd.m_ZAxis == binding)
				bnd.m_State.z = bnd.m_Mode == EAxisMode::Direct ? value * bnd.m_ZAxis.m_Sensitivity : value;
		}
	}

	void InputGroup::setButtonGroup(Binding binding, std::uint8_t state)
	{
		for (auto& bnd : m_ButtonBindings)
			if (bnd.m_Button == binding)
				bnd.m_State = state;

		if (state & ButtonStates::PressedMask)
		{
			for (auto& bnd : m_ButtonAxisBindings)
			{
				if (bnd.m_PosButton == binding)
					bnd.m_State += bnd.m_PosButton.m_Sensitivity;
				if (bnd.m_NegButton == binding)
					bnd.m_State -= bnd.m_NegButton.m_Sensitivity;
			}
			for (auto& bnd : m_ButtonAxis2DBindings)
			{
				if (bnd.m_PosXButton == binding)
					bnd.m_State.x += bnd.m_PosXButton.m_Sensitivity;
				if (bnd.m_NegXButton == binding)
					bnd.m_State.x -= bnd.m_NegXButton.m_Sensitivity;
				if (bnd.m_PosYButton == binding)
					bnd.m_State.y += bnd.m_PosYButton.m_Sensitivity;
				if (bnd.m_NegYButton == binding)
					bnd.m_State.y -= bnd.m_NegYButton.m_Sensitivity;
			}
			for (auto& bnd : m_ButtonAxis3DBindings)
			{
				if (bnd.m_PosXButton == binding)
					bnd.m_State.x += bnd.m_PosXButton.m_Sensitivity;
				if (bnd.m_NegXButton == binding)
					bnd.m_State.x -= bnd.m_NegXButton.m_Sensitivity;
				if (bnd.m_PosYButton == binding)
					bnd.m_State.y += bnd.m_PosYButton.m_Sensitivity;
				if (bnd.m_NegYButton == binding)
					bnd.m_State.y -= bnd.m_NegYButton.m_Sensitivity;
				if (bnd.m_PosZButton == binding)
					bnd.m_State.z += bnd.m_PosZButton.m_Sensitivity;
				if (bnd.m_NegZButton == binding)
					bnd.m_State.z -= bnd.m_NegZButton.m_Sensitivity;
			}
		}
	}

	float InputGroup::getButtonAxis(const Binding& pos, const Binding& neg) const
	{
		float value = 0.0f;
		if (m_Inputs->getButtonState(pos) & ButtonStates::DownMask)
			value += pos.m_Sensitivity;
		if (m_Inputs->getButtonState(neg) & ButtonStates::DownMask)
			value -= neg.m_Sensitivity;
		return value;
	}
} // namespace Brainfryer::Input