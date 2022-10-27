#pragma once

#include "Binding.h"
#include "Brainfryer/Utils/UID.h"

#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

namespace Brainfryer::Input
{
	class Inputs;

	class InputGroup
	{
	public:
		friend Inputs;

	public:
		InputGroup(Inputs* inputs, std::uint32_t id);
		virtual ~InputGroup() = default;

		void enable();
		void disable();
		void toggle();

		template <Bindable T>
		void registerBinding(UID id, T&& binding);

		template <Bindable T>
		T* getBinding(UID id);
		template <Bindable T>
		const T* getBinding(UID id) const;

		bool         getButton(UID id) const;
		std::uint8_t getButtonState(UID id) const;
		float        getAxis(UID id) const;
		glm::fvec2   getAxis2D(UID id) const;
		glm::fvec3   getAxis3D(UID id) const;

		auto getID() const { return m_ID; }
		bool isEnabled() const { return m_Enabled; }

	protected:
		void updateGroup();
		void postUpdateGroup();

		void setAxisGroup(Binding binding, float value);
		void setButtonGroup(Binding binding, std::uint8_t state);

	private:
		template <Bindable T>
		std::vector<BindingType<T::Type>>* getBindingVec();
		template <Bindable T>
		const std::vector<BindingType<T::Type>>* getBindingVec() const;

		float getButtonAxis(const Binding& pos, const Binding& neg) const;

	protected:
		Inputs*       m_Inputs;
		std::uint32_t m_ID;
		bool          m_Enabled;

		std::unordered_map<UID, BindingInfo> m_BindingsMap;

		std::vector<ButtonBinding>       m_ButtonBindings;
		std::vector<AxisBinding>         m_AxisBindings;
		std::vector<Axis2DBinding>       m_Axis2DBindings;
		std::vector<Axis3DBinding>       m_Axis3DBindings;
		std::vector<ButtonAxisBinding>   m_ButtonAxisBindings;
		std::vector<ButtonAxis2DBinding> m_ButtonAxis2DBindings;
		std::vector<ButtonAxis3DBinding> m_ButtonAxis3DBindings;
	};

	template <Bindable T>
	void InputGroup::registerBinding(UID id, T&& binding)
	{
		if (m_BindingsMap.contains(id))
			return;

		auto vec = getBindingVec<T>();
		m_BindingsMap.insert({ id, BindingInfo { T::Type, vec->size() } });
		vec->emplace_back(std::forward<T>(binding));
	}

	template <Bindable T>
	T* InputGroup::getBinding(UID id)
	{
		auto itr = m_BindingsMap.find(id);
		if (itr == m_BindingsMap.end())
			return nullptr;

		auto info = itr->second;
		if (info.m_Type != T::Type)
			return nullptr;

		auto vec = getBindingVec<T>();
		return info.m_Index < vec->size() ? &(*vec)[info.m_Index] : nullptr;
	}

	template <Bindable T>
	const T* InputGroup::getBinding(UID id) const
	{
		auto itr = m_BindingsMap.find(id);
		if (itr == m_BindingsMap.end())
			return nullptr;

		auto info = itr->second;
		if (info.m_Type != T::Type)
			return nullptr;

		auto vec = getBindingVec<T>();
		return info.m_Index < vec->size() ? &(*vec)[info.m_Index] : nullptr;
	}

	template <Bindable T>
	std::vector<BindingType<T::Type>>* InputGroup::getBindingVec()
	{
		if constexpr (T::Type == EBindingType::Button)
			return &m_ButtonBindings;
		else if constexpr (T::Type == EBindingType::Axis)
			return &m_AxisBindings;
		else if constexpr (T::Type == EBindingType::Axis2D)
			return &m_Axis2DBindings;
		else if constexpr (T::Type == EBindingType::Axis3D)
			return &m_Axis3DBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis)
			return &m_ButtonAxisBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis2D)
			return &m_ButtonAxis2DBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis3D)
			return &m_ButtonAxis3DBindings;
		else
			return nullptr;
	}

	template <Bindable T>
	const std::vector<BindingType<T::Type>>* InputGroup::getBindingVec() const
	{
		if constexpr (T::Type == EBindingType::Button)
			return &m_ButtonBindings;
		else if constexpr (T::Type == EBindingType::Axis)
			return &m_AxisBindings;
		else if constexpr (T::Type == EBindingType::Axis2D)
			return &m_Axis2DBindings;
		else if constexpr (T::Type == EBindingType::Axis3D)
			return &m_Axis3DBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis)
			return &m_ButtonAxisBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis2D)
			return &m_ButtonAxis2DBindings;
		else if constexpr (T::Type == EBindingType::ButtonAxis3D)
			return &m_ButtonAxis3DBindings;
		else
			return nullptr;
	}
} // namespace Brainfryer::Input