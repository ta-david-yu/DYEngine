#pragma once

#include "Core/EditorProperty.h"
#include "Math/Rect.h"
#include "Core/GUID.h"
#include "Util/Macro.h"

#include <toml++/toml.h>

namespace DYE::DYEditor
{
    struct SerializedArray
    {
        friend class SerializedComponent;

        SerializedArray() = default;

        inline std::size_t Size() const noexcept { return m_Array.size(); }

        template<typename T>
        std::optional<T> TryGetElementAtIndex(int index) const
        {
            toml::node const *pElementNode = m_Array.get(index);
            if (pElementNode == nullptr)
            {
                return {};
            }

            return pElementNode->value<T>();
        }

        template<typename T>
        void SetElementAtIndex(int index, T const &value)
        {
            DYE_ASSERT_LOG_WARN(index < Size(), "Tried to set element %d but it's out of range (array size: %d).", index, Size());
            m_Array.replace(m_Array.begin() + index, value);
        }

        template<typename T>
        void InsertElementAtIndex(int index, T const &value)
        {
            m_Array.insert(m_Array.begin() + index, value);
        }

    private:
        explicit SerializedArray(toml::array &&array);
        explicit SerializedArray(toml::array const &array);

        toml::array m_Array;
    };


    /// We need a GUID specialization because it's stored as string in serialized component.
    template<>
    std::optional<DYE::GUID> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<DYE::Vector2> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<DYE::Vector3> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<DYE::Vector4> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<DYE::Quaternion> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<DYE::AssetPath> SerializedArray::TryGetElementAtIndex(int index) const;
    template<>
    std::optional<Math::Rect> SerializedArray::TryGetElementAtIndex(int index) const;

    /// We need a GUID specialization because it's stored as string in serialized component.
    template<>
    void SerializedArray::SetElementAtIndex<DYE::GUID>(int index, DYE::GUID const &value);
    template<>
    void SerializedArray::SetElementAtIndex<DYE::Vector2>(int index, DYE::Vector2 const &value);
    template<>
    void SerializedArray::SetElementAtIndex<DYE::Vector3>(int index, DYE::Vector3 const &value);
    template<>
    void SerializedArray::SetElementAtIndex<DYE::Vector4>(int index, DYE::Vector4 const &value);
    template<>
    void SerializedArray::SetElementAtIndex<DYE::Quaternion>(int index, DYE::Quaternion const &value);
    template<>
    void SerializedArray::SetElementAtIndex<DYE::AssetPath>(int index, DYE::AssetPath const &value);
    template<>
    void SerializedArray::SetElementAtIndex<Math::Rect>(int index, Math::Rect const &value);

    /// We need a GUID specialization because it's stored as string in serialized component.
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::GUID>(int index, DYE::GUID const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::Vector2>(int index, DYE::Vector2 const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::Vector3>(int index, DYE::Vector3 const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::Vector4>(int index, DYE::Vector4 const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::Quaternion>(int index, DYE::Quaternion const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<DYE::AssetPath>(int index, DYE::AssetPath const &value);
    template<>
    void SerializedArray::InsertElementAtIndex<Math::Rect>(int index, Math::Rect const &value);
}