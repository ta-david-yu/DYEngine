#pragma once

#include "Util/Macro.h"

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

namespace DYE
{
    using BufferID = std::uint32_t;

	enum class BufferUsageHint
	{
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW,
		StreamDraw = GL_STREAM_DRAW
	};

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
		/// Create a VBO with a pre-allocated memory.
		/// \param size the size of the buffer (in byte)
		/// \return
		static std::shared_ptr<VertexBuffer> Create(std::uint32_t size, BufferUsageHint usage = BufferUsageHint::StaticDraw);

		/// Create a VBO with the given data.
		/// \param vertices the vertices data array
		/// \param size the size of the buffer (data array) (in byte)
		/// \return
		static std::shared_ptr<VertexBuffer> Create(void* vertices, std::uint32_t size, BufferUsageHint usage = BufferUsageHint::StaticDraw);

        VertexBuffer() = delete;

        /// Avoid using this constructor, use IndexBuffer::Create instead.
        explicit VertexBuffer(std::uint32_t size, BufferUsageHint usage);

        /// Avoid using this constructor, use IndexBuffer::Create instead.
        VertexBuffer(void* vertices, std::uint32_t size, BufferUsageHint usage);

        virtual ~VertexBuffer() = default;

        virtual void Bind() const;
        virtual void Unbind() const;

        /// Replace partial data of the buffer with new data.
		/// \param data a pointer to the data that will be copied.
		/// \param offset the start position offset where data replacement will begin.
		/// \param size the size of the data to be replaced.
        virtual void ReplaceData(const void* data, std::uint32_t offset, std::uint32_t size);

		/// Reset buffer with the given data.
		/// \param data a pointer to the data that will be copied from.
		/// \param size the size of the data to be copied from.
		virtual void ResetData(const void* data, std::uint32_t size, BufferUsageHint usage = BufferUsageHint::StaticDraw);

        /// Get the vertex attribute/element layout
        /// \return
        virtual const VertexLayout& GetLayout() const { return m_Layout; }

        /// Set the vertex attribute/element layout
        /// \param layout
        virtual void SetLayout(const VertexLayout& layout) { m_Layout = layout; }
    private:
        BufferID m_ID {};
        VertexLayout m_Layout;
    };

    /// Index Buffer class wrapper.
    /// Current implementation is in OpenGL
    class IndexBuffer
    {
    public:
		/// Create an Index Buffer with a pre-allocated memory.
		/// \param count the number of index elements in the array
		/// \return
		static std::shared_ptr<IndexBuffer> Create(std::uint32_t count, BufferUsageHint usage = BufferUsageHint::StaticDraw);

		/// Create an Index Buffer with the given indices.
		/// \param indices the indices data array
		/// \param count the number of index elements in the array
		/// \return
		static std::shared_ptr<IndexBuffer> Create(std::uint32_t* indices, std::uint32_t count, BufferUsageHint usage = BufferUsageHint::StaticDraw);

        IndexBuffer() = delete;

		/// Avoid using this constructor, use IndexBuffer::Create instead
		explicit IndexBuffer(std::uint32_t count, BufferUsageHint usage);

        /// Avoid using this constructor, use IndexBuffer::Create instead
        IndexBuffer(std::uint32_t* indices, std::uint32_t count, BufferUsageHint usage);

        virtual ~IndexBuffer() = default;

        virtual void Bind() const;
        virtual void Unbind() const;

		void ResetData(std::uint32_t *indices, std::uint32_t count, BufferUsageHint usage = BufferUsageHint::StaticDraw);

        std::uint32_t GetCount() const { return m_IndicesCount; };

    private:
        BufferID m_ID {};
        std::uint32_t m_IndicesCount {};
    };
}
