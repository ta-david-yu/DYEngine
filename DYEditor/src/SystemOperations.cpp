#include "Undo/Operations/SystemOperations.h"

#include "Core/Scene.h"

#include <vector>

namespace DYE::DYEditor
{
    // SystemAdditionOperation

    void SystemAdditionOperation::Undo()
    {
        std::vector<SystemDescriptor> &systemDescriptors =
            Descriptor.Instance == nullptr ? pScene->UnrecognizedSystems : pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);

        systemDescriptors.erase(systemDescriptors.begin() + OrderInSystemList);
    }

    void SystemAdditionOperation::Redo()
    {
        std::vector<SystemDescriptor> &systemDescriptors =
            Descriptor.Instance == nullptr ? pScene->UnrecognizedSystems : pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);

        systemDescriptors.insert(systemDescriptors.begin() + OrderInSystemList, Descriptor);
    }

    // SystemRemovalOperation

    void SystemRemovalOperation::Undo()
    {
        std::vector<SystemDescriptor> &systemDescriptors =
            Descriptor.Instance == nullptr ? pScene->UnrecognizedSystems : pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);

        systemDescriptors.insert(systemDescriptors.begin() + OrderInSystemList, Descriptor);
    }

    void SystemRemovalOperation::Redo()
    {
        std::vector<SystemDescriptor> &systemDescriptors =
            Descriptor.Instance == nullptr ? pScene->UnrecognizedSystems : pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);

        systemDescriptors.erase(systemDescriptors.begin() + OrderInSystemList);
    }

    // SetSystemIsEnabledOperation

    void SetSystemIsEnabledOperation::Undo()
    {
        auto &systemList = pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);
        systemList[OrderInList].IsEnabled = !IsEnabledValue;
    }

    void SetSystemIsEnabledOperation::Redo()
    {
        auto &systemList = pScene->GetSystemDescriptorsOfPhase(ExecutionPhase);
        systemList[OrderInList].IsEnabled = IsEnabledValue;
    }

    // SystemReorderOperation

    void SystemReorderOperation::Undo()
    {
        auto &systemDescriptors = pScene->GetSystemDescriptorsOfPhase(pSystemBase->GetPhase());
        auto const otherDescriptor = systemDescriptors[OrderBeforeModification];
        systemDescriptors[OrderBeforeModification] = systemDescriptors[OrderAfterModification];
        systemDescriptors[OrderAfterModification] = otherDescriptor;
    }

    void SystemReorderOperation::Redo()
    {
        auto &systemDescriptors = pScene->GetSystemDescriptorsOfPhase(pSystemBase->GetPhase());
        auto const otherDescriptor = systemDescriptors[OrderAfterModification];
        systemDescriptors[OrderAfterModification] = systemDescriptors[OrderBeforeModification];
        systemDescriptors[OrderBeforeModification] = otherDescriptor;
    }
}