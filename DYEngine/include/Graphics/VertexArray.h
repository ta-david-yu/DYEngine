#pragma once

#include "Graphics/Buffer.h"

#include <memory>
#include <vector>

namespace DYE
{
    using VertexArrayID = std::uint32_t;

    class VertexArray
    {
    public:
        VertexArray();
        virtual ~VertexArray();

        static std::shared_ptr<VertexArray> Create();

        virtual void Bind() const;
        virtual void Unbind() const;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
    private:
        VertexArrayID m_ID {0};
        /// The latest index of the vertex array attributes
        BufferID m_VertexBufferAttributeIndex {0};
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

    };
}