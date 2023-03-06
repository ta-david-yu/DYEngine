#include "../include/EntityLevelFile.h"

namespace DYE::DYEntity
{
	std::shared_ptr<EntityLevelFile> EntityLevelFile::CreateFromFile(const std::filesystem::path &filepath)
	{
		// TODO:
		return std::make_shared<EntityLevelFile>();
	}
}
