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

		for (int i = 0; i < systemDescriptors.size(); ++i)
		{
			SystemDescriptor & descriptor = systemDescriptors[i];
			if (descriptor.Name == Descriptor.Name)
			{
				systemDescriptors.erase(systemDescriptors.begin() + i);
				break;
			}
		}
	}

	void SystemAdditionOperation::Redo()
	{
		if (Descriptor.Instance == nullptr)
		{
			// The added system is an unrecognized system.
			pScene->UnrecognizedSystems.push_back(Descriptor);
		}
		else
		{
			pScene->GetSystemDescriptorsOfPhase(ExecutionPhase).push_back(Descriptor);
		}
	}

	// SystemRemovalOperation

	void SystemRemovalOperation::Undo()
	{

	}

	void SystemRemovalOperation::Redo()
	{

	}

	// ToggleSystemOperation

	void ToggleSystemOperation::Undo()
	{

	}

	void ToggleSystemOperation::Redo()
	{

	}
}