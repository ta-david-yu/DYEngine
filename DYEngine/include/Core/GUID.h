#pragma once

#include <string>
#include <functional>
#include <cstdint>

namespace DYE
{
	struct GUID
	{
		friend class GUIDFactory;
		GUID() = default;
		explicit GUID(std::uint64_t guid) : m_GUID(guid) { }
		bool IsValid() const { return m_GUID != 0; }

		explicit operator std::uint64_t() const { return m_GUID; }
		std::string ToString() const { return std::to_string(m_GUID); }
	private:
		std::uint64_t m_GUID = 0;
	};

	class GUIDFactory
	{
	public:
		// Although this function works statically right now (i.e. all factory instance using the same random generator),
		// it is not marked as static. Because at some point we want to support multiple factory instances for different systems
		// instead of one static instance only.
		// For instance, scene/prefab local identifier & asset identifier etc
		GUID Generate();
	};
}

namespace std
{
	template<>
	struct hash<DYE::GUID>
	{
		std::size_t operator()(DYE::GUID const& guid) const
		{
			return hash<std::uint64_t>()((std::uint64_t)guid);
		}
	};
}