#pragma once

#include "Entity.h"
#include "ImGui/ImGuiUtil.h"

#include <concepts>

namespace DYE::DYEditor
{
	template<typename T>
	bool DefaultHasComponentOfType(DYE::DYEntity::Entity& entity)
	{
		return entity.HasComponent<T>();
	}

	template<typename T>
	void DefaultAddComponentOfType(DYE::DYEntity::Entity& entity)
	{
		// By default, add component to the entity using default constructor
		entity.AddComponent<T>();
	}

	template<typename T>
	void DefaultRemoveComponentOfType(DYE::DYEntity::Entity& entity)
	{
		entity.RemoveComponent<T>();
	}

	template<typename T>
	/// A concept that checks if the given type has a public member variable of type 'bool' with the name of 'IsEnabled'
	concept HasIsEnabled = requires(T instance)
	{
		{ instance.IsEnabled } -> std::same_as<bool>;
	};

	template<typename T>
	bool DefaultDrawComponentHeaderWithIsEnabled(DYE::DYEntity::Entity &entity, bool &isHeaderVisible, bool &entityChanged, std::string const &headerLabel)
	{
		// I wanted to use the 'HasIsEnabled' concept to do static_assert when the given type doesn't have a 'IsEnabled' bool member variable.
		// But for some reasons it never works as I expected, so I would just leave it like this :P
		// This is an internal syntactic sugar function anyway, so no one other than I would use it in rare occasions.

		//static_assert(HasIsEnabled<T>, "Type T does not have a public member variable named IsEnabled of type bool.");

		ImGuiTreeNodeFlags const flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
		bool const showInspector = ImGui::CollapsingHeader("##Header", &isHeaderVisible, flags);

		ImGui::SameLine();
		entityChanged |= ImGui::Checkbox("##IsEnabledCheckbox", &entity.GetComponent<T>().IsEnabled);

		ImGui::SameLine();
		ImGui::TextUnformatted(headerLabel.c_str());

		return showInspector;
	}
}