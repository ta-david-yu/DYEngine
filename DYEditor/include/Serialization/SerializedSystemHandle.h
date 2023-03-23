#pragma once

#include <string>
#include <optional>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	constexpr const char* SystemTypeNameKey = "Type";

	struct SerializedSystemHandle
	{
		friend class SerializedScene;
		SerializedSystemHandle() = delete;

		std::optional<std::string> TryGetTypeName() const;
		void SetTypeName(std::string const& typeName);

	private:
		explicit SerializedSystemHandle(toml::table* pSystemTable);
		toml::table* m_pSystemTable = nullptr;
	};
}