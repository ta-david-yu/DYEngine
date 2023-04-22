#include "Core/GUID.h"

#include <random>

namespace DYE
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<std::uint64_t> s_UniformDistribution;

	GUID GUIDFactory::Generate()
	{
		auto newGUID = s_UniformDistribution(s_Engine);
		if (newGUID == 0)
		{
			// Regenerate once to lower the chance of it being 0.
			newGUID = s_UniformDistribution(s_Engine);
		}
		return GUID(newGUID);
	}
}