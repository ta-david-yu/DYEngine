#include "Configuration/SubWindowConfiguration.h"

#include "Configuration/ProjectConfig.h"
#include "Graphics/WindowManager.h"
#include "Graphics/WindowBase.h"
#include "Util/Logger.h"

namespace DYE::DYEditor
{
    struct SubWindowConfigurationData
    {
        bool IsSetup = false;
        std::vector<WindowID> OpenSubWindowIDs;
    };

    static SubWindowConfigurationData s_Data;

    void SetupSubWindowsBasedOnRuntimeConfig()
    {
        if (s_Data.IsSetup)
        {
            DYE_LOG("Runtime Config SubWindows has already been setup! You cannot setup again before it is cleared.");
            return;
        }

        s_Data.IsSetup = true;

        ProjectConfig &config = GetRuntimeConfig();
        toml::node *pArrayOfSubWindowsTableNode = config.Table().get(RuntimeConfigKeys::SubWindows);
        if (pArrayOfSubWindowsTableNode == nullptr)
        {
            // Array of sub-windows tables don't exist yet, insert one!
            config.Table().insert(RuntimeConfigKeys::SubWindows, toml::array {});
            pArrayOfSubWindowsTableNode = config.Table().get(RuntimeConfigKeys::SubWindows);
        }

        toml::array *pArrayOfSubWindowsTable = pArrayOfSubWindowsTableNode->as_array();
        for (int i = 0; i < pArrayOfSubWindowsTable->size(); ++i)
        {
            toml::node *pSubWindowNode = pArrayOfSubWindowsTable->get(i);
            toml::table *pSubWindowTable = pSubWindowNode->as_table();

            auto &windowName = pSubWindowTable->get("Name")->as_string()->get();
            int windowWidth = pSubWindowTable->get("Width")->as_integer()->get();
            int windowHeight = pSubWindowTable->get("Height")->as_integer()->get();

            auto subWindowPtr = WindowManager::CreateWindow(WindowProperties(windowName, windowWidth, windowHeight));
            subWindowPtr->SetContext(WindowManager::GetMainWindow()->GetContext());

            s_Data.OpenSubWindowIDs.push_back(subWindowPtr->GetWindowID());
        }
    }

    void ClearSubWindowsBasedOnRuntimeConfig()
    {
        if (!s_Data.IsSetup)
        {
            DYE_LOG("Runtime Config has not been setup! No need to clear them.");
            return;
        }

        for (auto windowId: s_Data.OpenSubWindowIDs)
        {
            WindowManager::CloseWindow(windowId);
        }

        s_Data.IsSetup = false;
        s_Data.OpenSubWindowIDs.clear();
    }
}