#include <memory>

namespace DYE
{
    using BufferID = std::uint32_t;

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

        virtual void SetData(const void* data, std::uint32_t size);

        //virtual const BufferLayout& GetLayout() const = 0;
        //virtual void SetLayout(const BufferLayout& layout) = 0;

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
