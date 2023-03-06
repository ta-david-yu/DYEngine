#pragma once

#include <string>
#include <filesystem>
#include <memory>

namespace DYE::DYEntity
{
	// A memory representation of a DYEntity level file.
	// It stores which layer the level uses and world information (i.e. entities & components).
	class EntityLevelFile
	{
	public:
		static std::shared_ptr<EntityLevelFile> CreateFromFile(const std::filesystem::path& filepath);

		EntityLevelFile() = default;

		/// \return the name of the layer to be used for the level.
		std::string GetLevelLayerName() const
		{
			// TODO: extra
			return "";
		}

		/* TODO:
		 * // populate the given world with the entities & components
		 * void PopulateWorld(World& world);
		 *
		 */
	};
}