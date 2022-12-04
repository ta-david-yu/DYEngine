#pragma once

#include "Base.h"

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

namespace DYE
{
    using BufferID = std::uint32_t;

    enum class VertexAttributeType
    {
        None = -1,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
    };

    static std::uint32_t VertexAttributeTypeSize(VertexAttributeType type)
    {
        switch (type)
        {
            case VertexAttributeType::Float:    return 4;
            case VertexAttributeType::Float2:   return 4 * 2;
            case VertexAttributeType::Float3:   return 4 * 3;
            case VertexAttributeType::Float4:   return 4 * 4;
            case VertexAttributeType::Mat3:     return 4 * 3 * 3;
            case VertexAttributeType::Mat4:     return 4 * 4 * 4;
            case VertexAttributeType::Int:      return 4;
            case VertexAttributeType::Int2:     return 4 * 2;
            case VertexAttributeType::Int3:     return 4 * 3;
            case VertexAttributeType::Int4:     return 4 * 4;
            case VertexAttributeType::Bool:     return 1;

            case VertexAttributeType::None:
                break;
        }

        DYE_LOG_ERROR("Unknown VertexAttributeType!");
        DYE_ASSERT(false);
        return 0;
    }

    /// Vertex Attribute: an element/attribute of vertices
    struct VertexAttribute
    {
        std::string Name;
        VertexAttributeType Type;
        uint32_t Size;
        size_t Offset;
        bool Normalized;

        VertexAttribute() = default;

        VertexAttribute(VertexAttributeType type, const std::string& name, bool normalized = false)
                : Name(name), Type(type), Size(VertexAttributeTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
                case VertexAttributeType::Float:   return 1;
                case VertexAttributeType::Float2:  return 2;
                case VertexAttributeType::Float3:  return 3;
                case VertexAttributeType::Float4:  return 4;
                case VertexAttributeType::Mat3:    return 3; // 3 * float3
                case VertexAttributeType::Mat4:    return 4; // 4 * float4
                case VertexAttributeType::Int:     return 1;
                case VertexAttributeType::Int2:    return 2;
                case VertexAttributeType::Int3:    return 3;
                case VertexAttributeType::Int4:    return 4;
                case VertexAttributeType::Bool:    return 1;

                case VertexAttributeType::None:
                    break;
            }

            DYE_LOG_ERROR("Unknown VertexAttributeType!");
            DYE_ASSERT(false);
            return 0;
        }
    };

    /// Vertex Buffer Layout: contains a layout of attributes per vertex
    class VertexLayout
    {
    public:
        VertexLayout() {}
        ///
        /// \param elements a list of elements
        VertexLayout(std::initializer_list<VertexAttribute> elements) : m_Elements(elements)
        {
            calculateOffsetsAndStride();
        }

        std::uint32_t GetStride() const { return m_Stride; }
        const std::vector<VertexAttribute>& GetElements() const { return m_Elements; }

        std::vector<VertexAttribute>::iterator begin() { return m_Elements.begin(); }
        std::vector<VertexAttribute>::iterator end() { return m_Elements.end(); }
        std::vector<VertexAttribute>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<VertexAttribute>::const_iterator end() const { return m_Elements.end(); }
    private:
        void calculateOffsetsAndStride()
        {
            size_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<VertexAttribute> m_Elements;
        std::uint32_t m_Stride = 0;
    };

    /// Vertex Buffer class wrapper.
    /// Current implementation is in OpenGL
    class VertexBuffer
    {
    public:
        VertexBuffer() = delete;
        /// Avoid using this constructor, use IndexBuffer::Create instead
        explicit VertexBuffer(std::uint32_t size);
        /// Avoid using this constructor, use IndexBuffer::Create instead
        VertexBuffer(void* vertices, std::uint32_t size);
        virtual ~VertexBuffer() = default;

        virtual void Bind() const;
        virtual void Unbind() const;

        /// Set the vertex data
        /// \param data a pointer to the vertices data
        /// \param size the size of the data
        virtual void SetData(const void* data, std::uint32_t size);

        /// Get the vertex attribute/element layout
        /// \return
        virtual const VertexLayout& GetLayout() const { return m_Layout; }

        /// Set the vertex attribute/element layout
        /// \param layout
        virtual void SetLayout(const VertexLayout& layout) { m_Layout = layout; }

        ///
        /// \param size the size of the buffer (in byte)
        /// \return
        static std::shared_ptr<VertexBuffer> Create(std::uint32_t size);
        ///
        /// \param vertices the vertices data array
        /// \param size the size of the buffer (data array) (in byte)
        /// \return
        static std::shared_ptr<VertexBuffer> Create(void* vertices, std::uint32_t size);
    private:
        BufferID m_ID {};
        VertexLayout m_Layout;
    };

    /// Index Buffer class wrapper.
    /// Current implementation is in OpenGL
    class IndexBuffer
    {
    public:
        IndexBuffer() = delete;
        /// Avoid using this constructor, use IndexBuffer::Create instead
        IndexBuffer(std::uint32_t* indices, uint32_t count);
        virtual ~IndexBuffer() = default;

        virtual void Bind() const;
        virtual void Unbind() const;

        std::uint32_t GetCount() const { return m_IndicesCount; };

        ///
        /// \param indices the indices data array
        /// \param count the number of index elements in the array
        /// \return
        static std::shared_ptr<IndexBuffer> Create(std::uint32_t* indices, std::uint32_t count);
    private:
        BufferID m_ID {};
        std::uint32_t m_IndicesCount {};
    };
}
