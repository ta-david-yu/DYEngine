#include "Graphics/VertexArray.h"
#include "Base.h"


namespace DYE
{
    std::shared_ptr<VertexArray> VertexArray::Create()
    {
        return std::make_shared<VertexArray>();
    }

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_ID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_ID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_ID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
    {
        if (vertexBuffer->GetLayout().GetElements().size() == 0)
        {
            DYE_LOG("Vertex Buffer has no layout!");
            DYE_ASSERT(true);
        }
        glBindVertexArray(m_ID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            switch (element.Type)
            {
                /// Cast to float type (glVertexAttribPointer)
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                {
                    glEnableVertexAttribArray(m_VertexBufferAttributeIndex);
                    glVertexAttribPointer(m_VertexBufferAttributeIndex,
                                          element.GetComponentCount(),
                                          ShaderDataTypeToOpenGLBaseType(element.Type),
                                          element.Normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.Offset);
                    m_VertexBufferAttributeIndex++;
                    break;
                }
                /// Cast to integer type (glVertexAttribIPointer)
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    glEnableVertexAttribArray(m_VertexBufferAttributeIndex);
                    glVertexAttribIPointer(m_VertexBufferAttributeIndex,
                                           element.GetComponentCount(),
                                           ShaderDataTypeToOpenGLBaseType(element.Type),
                                           layout.GetStride(),
                                           (const void*)element.Offset);
                    m_VertexBufferAttributeIndex++;
                    break;
                }
                /// Cast to float type matrix,  (glVertexAttribPointer)
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(m_VertexBufferAttributeIndex);
                        glVertexAttribPointer(m_VertexBufferAttributeIndex,
                                              count,
                                              ShaderDataTypeToOpenGLBaseType(element.Type),
                                              element.Normalized ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(),
                                              (const void*)(element.Offset + sizeof(float) * count * i));
                        glVertexAttribDivisor(m_VertexBufferAttributeIndex, 1);
                        m_VertexBufferAttributeIndex++;
                    }
                    break;
                }
                default:
                    DYE_LOG("Unknown ShaderDataType!");
                    DYE_ASSERT(false);
                    break;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
    {

    }
}