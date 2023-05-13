#pragma once

#include <filesystem>

namespace DYE::DYEditor
{
	struct LoadSceneComponent
	{
		std::filesystem::path SceneAssetPath = "";
	};
}