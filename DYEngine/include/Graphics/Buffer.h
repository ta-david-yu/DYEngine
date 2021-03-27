#pragma once

#include "Base.h"

#include <glad/glad.h>

#include <memory>
#include <vector>

namespace DYE
{
    using BufferID = std::uint32_t;

    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:    return GL_FLOAT;
            case ShaderDataType::Float2:   return GL_FLOAT;
            case ShaderDataType::Float3:   return GL_FLOAT;
            case ShaderDataType::Float4:   return GL_FLOAT;
            case ShaderDataType::Mat3:     return GL_FLOAT;
            case ShaderDataType::Mat4:     return GL_FLOAT;
            case ShaderDataType::Int:      return GL_INT;
            case ShaderDataType::Int2:     return GL_INT;
            case ShaderDataType::Int3:     return GL_INT;
            case ShaderDataType::Int4:     return GL_INT;
            case ShaderDataType::Bool:     return GL_BOOL;

            case ShaderDataType::None:
                break;
        }

        DYE_LOG_ERROR("Unknown ShaderDataType!");
        DYE_ASSERT(false);
        return 0;
    }

    static std::uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:    return 4;
            case ShaderDataType::Float2:   return 4 * 2;
            case ShaderDataType::Float3:   return 4 * 3;
            case ShaderDataType::Float4:   return 4 * 4;
            case ShaderDataType::Mat3:     return 4 * 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4 * 4;
            case ShaderDataType::Int:      return 4;
            case ShaderDataType::Int2:     return 4 * 2;
            case ShaderDataType::Int3:     return 4 * 3;
            case ShaderDataType::Int4:     return 4 * 4;
            case ShaderDataType::Bool:     return 1;

            case ShaderDataType::None:
                break;
        }

        DYE_LOG_ERROR("Unknown ShaderDataType!");
        DYE_ASSERT(false);
        return 0;
    }

    /// Vertex Element: an element/attribute of vertices
    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        size_t Offset;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
                : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3; // 3 * float3
                case ShaderDataType::Mat4:    return 4; // 4 * float4
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;

                case ShaderDataType::None:
                    break;
            }

            DYE_LOG_ERROR("Unknown ShaderDataType!");
            DYE_ASSERT(false);
            return 0;
        }
    };

    /// Vertex Buffer Layout: contains a layout of elements per vertex
    class BufferLayout
    {
    public:
        BufferLayout() {}
        ///
        /// \param elements a list of elements
        BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements)
        {
            calculateOffsetsAndStride();
        }

        std::uint32_t GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
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
        std::vector<BufferElement> m_Elements;
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
        virtual const BufferLayout& GetLayout() const { return m_Layout; }

        /// Set the vertex attribute/element layout
        /// \param layout
        virtual void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

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
        BufferLayout m_Layout;
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
