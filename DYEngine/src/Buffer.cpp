#include "Graphics/Buffer.h"
#include "Graphics/OpenGL.h"

namespace DYE
{
    VertexBuffer::VertexBuffer(std::uint32_t size)
    {
        glCall(glGenBuffers(1, &m_ID));
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::VertexBuffer(void *vertices, std::uint32_t size)
    {
        glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void *data, std::uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(std::uint32_t size)
    {
        return std::make_shared<VertexBuffer>(size);
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(void *vertices, std::uint32_t size)
    {
        return std::make_shared<VertexBuffer>(vertices, size);
    }

    /* -- Index Buffer -- */

    IndexBuffer::IndexBuffer(std::uint32_t *indices, std::uint32_t count) : m_IndicesCount(count)
    {
        glGenBuffers(1, &m_ID);

        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
        // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(std::uint32_t), indices, GL_STATIC_DRAW);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(std::uint32_t *indices, std::uint32_t count)
    {
        return std::make_shared<IndexBuffer>(indices, count);
    }
}