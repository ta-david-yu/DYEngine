#include "Serialization/SerializedArray.h"

namespace DYE::DYEditor
{
	SerializedArray::SerializedArray(toml::array &&array) : m_Array(array)
	{

	}

	SerializedArray::SerializedArray(toml::array const &array) : m_Array(array)
	{

	}

	/// We need a GUID specialization because it's stored as string in serialized component.
	template<>
	std::optional<DYE::GUID> SerializedArray::TryGetElementAtIndex(int index) const
	{
		auto guidString = m_Array.get(index)->value<std::string>();
		if (!guidString.has_value())
		{
			return {};
		}

		try
		{
			std::uint64_t guidInt = std::stoul(guidString.value());
			return GUID(guidInt);
		}
		catch (const std::exception& e)
		{
			return {};
		}
	}

	template<>
	std::optional<DYE::Vector2> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	std::optional<DYE::Vector3> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	std::optional<DYE::Vector4> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	std::optional<DYE::Quaternion> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	std::optional<DYE::AssetPath> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	std::optional<Math::Rect> SerializedArray::TryGetElementAtIndex(int index) const
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}


	/// We need a GUID specialization because it's stored as string in serialized component.
	template<>
	void SerializedArray::SetElementAtIndex<DYE::GUID>(int index, DYE::GUID const& value)
	{
		m_Array.replace(m_Array.begin() + index, value.ToString());
	}

	template<>
	void SerializedArray::SetElementAtIndex<DYE::Vector2>(int index, DYE::Vector2 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::SetElementAtIndex<DYE::Vector3>(int index, DYE::Vector3 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::SetElementAtIndex<DYE::Vector4>(int index, DYE::Vector4 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::SetElementAtIndex<DYE::Quaternion>(int index, DYE::Quaternion const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::SetElementAtIndex<DYE::AssetPath>(int index, DYE::AssetPath const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::SetElementAtIndex<Math::Rect>(int index, Math::Rect const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	/// We need a GUID specialization because it's stored as string in serialized component.
	template<>
	void SerializedArray::InsertElementAtIndex<DYE::GUID>(int index, DYE::GUID const& value)
	{
		m_Array.insert(m_Array.begin() + index, value.ToString());
	}

	template<>
	void SerializedArray::InsertElementAtIndex<DYE::Vector2>(int index, DYE::Vector2 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::InsertElementAtIndex<DYE::Vector3>(int index, DYE::Vector3 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::InsertElementAtIndex<DYE::Vector4>(int index, DYE::Vector4 const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::InsertElementAtIndex<DYE::Quaternion>(int index, DYE::Quaternion const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::InsertElementAtIndex<DYE::AssetPath>(int index, DYE::AssetPath const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}

	template<>
	void SerializedArray::InsertElementAtIndex<Math::Rect>(int index, Math::Rect const& value)
	{
		DYE_ASSERT_LOG_WARN(false, "Not implemented!");
	}
}