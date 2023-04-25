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

	// ToggleSystemOperation

	void ToggleSystemOperation::Undo()
	{

	}

	void ToggleSystemOperation::Redo()
	{

	}
}