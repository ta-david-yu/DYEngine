#pragma once

#include "Core/EditorProperty.h"
#include "World.h"

#include <optional>

/// A class marked with this macro will be identified by DYEditor code generator. DYEditor code generator will
/// then generate code that registers the marked class into TypeRegistry as a system. \n\n
/// Example:\n
/// DYE_SYSTEM("Foo", SystemFoo) \n
/// struct SystemFoo final : public SystemBase { //... } \n\n
/// Notice that the class should always be derived from SystemBase class.
///
/// \param systemType the system type. You should include namespace(s) as if the type is being accessed in global scope.
/// \param systemName the string name that will be used to identify the system in DYEditor.
/// \param optionalParameters ...to define tooltip etc
#define DYE_SYSTEM(systemType, systemName, ...)

namespace DYE::DYEditor
{
    enum class ExecutionMode
    {
        Play,
        Edit
    };

    enum class ExecutionPhase
    {
        /// Called at the start of the scene (after scene loaded)
        Initialize = 0,
        FixedUpdate,
        Update,
        /// Called after all Updates but before all Renders
        LateUpdate,
        /// Called after all LateUpdates in Runtime Mode. It's also called in Edit Mode to render the scene in scene view.
        Render,
        /// It's also called in Edit Mode to render the scene in scene view.
        PostRender,
        ImGui,
        /// Called at the end of each frame
        Cleanup,
        /// Called at the end of the scene (before scene is unloaded)
        TearDown
    };

    std::string CastExecutionPhaseToString(ExecutionPhase phase);

    std::optional<ExecutionPhase> TryCastStringToExecutionPhase(std::string_view const &phaseInString);

    struct ExecuteParameters
    {
        ExecutionMode Mode = ExecutionMode::Play;
        ExecutionPhase Phase = ExecutionPhase::Update;
    };

    enum class InitializeLoadType
    {
        Manual,
        AfterLoadScene,

        // This will only happen in Editor.
        BeforeEnterPlayMode,
        // This will only happen in Editor.
        BeforeEnterEditMode
    };

    struct InitializeLoadParameters
    {
        InitializeLoadType LoadType = InitializeLoadType::AfterLoadScene;
    };

    struct SystemBase
    {
        virtual bool ExecuteInEditMode() const { return false; }
        virtual ExecutionPhase GetPhase() const = 0;
        virtual void InitializeLoad(DYE::DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters) {}
        virtual void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) = 0;
        virtual void DrawInspector(DYE::DYEditor::World &world);
        virtual ~SystemBase() = default;
    };
}