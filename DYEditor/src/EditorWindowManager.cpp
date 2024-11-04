#include "ImGui/EditorWindowManager.h"

namespace DYE::DYEditor
{
    std::vector<EditorWindow> EditorWindowManager::s_EditorWindows = {};

    void EditorWindowManager::DrawEditorWindows(ImGuiViewport const *pMainViewportHint)
    {
        for (auto &windowDescriptor: s_EditorWindows)
        {
            if (!windowDescriptor.IsConfigOpen)
            {
                continue;
            }

            windowDescriptor.BodyFunction(windowDescriptor.Name, &windowDescriptor.IsConfigOpen, pMainViewportHint);
        }
    }
}