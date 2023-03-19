#pragma once

#include "EditorProperty.h"
#include "World.h"

/// A class marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class into TypeRegistry as a system. \n\n
/// Example:\n
/// DYE_SYSTEM("Foo", SystemFoo) \n
/// struct SystemFoo final : public SystemBase { //... } \n\n
/// Notice that the class should always be derived from SystemBase class.
/// \param systemName the string name that will be used to identify the system in DYEditor.
/// \param systemType the system type. You should include namespace(s) as if the type is being accessed in global scope.
/// \param optionalParameters ...to define tooltip etc
#define DYE_SYSTEM(systemName, systemType, ...)

namespace DYE::DYEditor
{
	enum class ExecutionPhase
	{
		Initialize = 0,
		Update,
		FixedUpdate,
		Render,
		ImGui
	};

	struct ExecuteParameters
	{
		ExecutionPhase Phase = ExecutionPhase::Update;
	};

	struct SystemBase
	{
		virtual ExecutionPhase GetPhase() const = 0;
		virtual void Execute(DYE::DYEntity::World& world, DYE::DYEditor::ExecuteParameters params) = 0;

		virtual void DrawInspector(DYE::DYEntity::World& world);
		virtual ~SystemBase() = default;
	};
}