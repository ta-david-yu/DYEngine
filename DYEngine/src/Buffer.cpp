#include "Graphics/Buffer.h"
#include "Graphics/OpenGL.h"

namespace DYE
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(std::uint32_t size, BufferUsageHint usage)
	{
		return std::make_shared<VertexBuffer>(size, usage);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void *vertices, std::uint32_t size, BufferUsageHint usage)
	{
		return std::make_shared<VertexBuffer>(vertices, size, usage);
	}

    VertexBuffer::VertexBuffer(std::uint32_t size, BufferUsageHint usage)
    {
        glCall(glCreateBuffers(1, &m_ID));
        glCall(glNamedBufferData(m_ID, size, nullptr, static_cast<GLenum>(usage)));
    }

    VertexBuffer::VertexBuffer(void *vertices, std::uint32_t size, BufferUsageHint usage)
    {
        glCall(glCreateBuffers(1, &m_ID));
        glCall(glNamedBufferData(m_ID, size, vertices, static_cast<GLenum>(usage)));
    }

    void VertexBuffer::Bind() const
    {
        glCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
    }

    void VertexBuffer::Unbind() const
    {
        glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VertexBuffer::ReplaceData(const void *data, std::uint32_t offset, std::uint32_t size)
    {
		glCall(glNamedBufferSubData(m_ID, offset, size, data));
    }

	void VertexBuffer::ResetData(const void *data, std::uint32_t size, BufferUsageHint usage)
	{
		glNamedBufferData(m_ID, size, data, static_cast<GLenum>(usage));
	}

	/* -- Index Buffer -- */

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(std::uint32_t count, BufferUsageHint usage)
	{
		return std::make_shared<IndexBuffer>(count, usage);
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(std::uint32_t *indices, std::uint32_t count, BufferUsageHint usage)
	{
		return std::make_shared<IndexBuffer>(indices, count, usage);
	}

	IndexBuffer::IndexBuffer(std::uint32_t count, BufferUsageHint usage) : m_IndicesCount(count)
	{
		glCall(glCreateBuffers(1, &m_ID));

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		glCall(glNamedBufferData(m_ID, count * sizeof(std::uint32_t), nullptr, static_cast<GLenum>(usage)));
	}

    IndexBuffer::IndexBuffer(std::uint32_t *indices, std::uint32_t count, BufferUsageHint usage) : m_IndicesCount(count)
    {
        glCall(glCreateBuffers(1, &m_ID));

        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
        // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        glCall(glNamedBufferData(m_ID, count * sizeof(std::uint32_t), indices, static_cast<GLenum>(usage)));
    }

    void IndexBuffer::Bind() const
    {
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
    }

    void IndexBuffer::Unbind() const
    {
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

	void IndexBuffer::ResetData(std::uint32_t *indices, std::uint32_t count, BufferUsageHint usage)
	{
		glCall(glNamedBufferData(m_ID, count * sizeof(std::uint32_t), indices, static_cast<GLenum>(usage)));
		m_IndicesCount = count;
	}
}