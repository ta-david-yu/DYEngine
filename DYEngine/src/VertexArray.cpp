#include "Graphics/VertexArray.h"
#include "Util/Macro.h"
#include "Graphics/OpenGL.h"


namespace DYE
{
    static GLenum ShaderDataTypeToOpenGLBaseType(VertexAttributeType type)
    {
        switch (type)
        {
            case VertexAttributeType::Float:    return GL_FLOAT;
            case VertexAttributeType::Float2:   return GL_FLOAT;
            case VertexAttributeType::Float3:   return GL_FLOAT;
            case VertexAttributeType::Float4:   return GL_FLOAT;
            case VertexAttributeType::Mat3:     return GL_FLOAT;
            case VertexAttributeType::Mat4:     return GL_FLOAT;
            case VertexAttributeType::Int:      return GL_INT;
            case VertexAttributeType::Int2:     return GL_INT;
            case VertexAttributeType::Int3:     return GL_INT;
            case VertexAttributeType::Int4:     return GL_INT;
            case VertexAttributeType::Bool:     return GL_BOOL;

            case VertexAttributeType::None:
                break;
        }

        DYE_LOG_ERROR("Unknown VertexAttributeType!");
        DYE_ASSERT(false);
        return 0;
    }

    std::shared_ptr<VertexArray> VertexArray::Create()
    {
        return std::make_shared<VertexArray>();
    }

    VertexArray::VertexArray()
    {
        glCall(glGenVertexArrays(1, &m_ID));
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
        if (vertexBuffer->GetLayout().GetElements().empty())
        {
            DYE_LOG("Vertex Buffer has no layout!");
            DYE_ASSERT(false);
        }

        glCall(glBindVertexArray(m_ID));
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            switch (element.Type)
            {
                /// Cast to float type (glVertexAttribPointer)
                case VertexAttributeType::Float:
                case VertexAttributeType::Float2:
                case VertexAttributeType::Float3:
                case VertexAttributeType::Float4:
                {
                    glCall(glEnableVertexAttribArray(m_VertexBufferAttributeIndex));
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
                case VertexAttributeType::Int:
                case VertexAttributeType::Int2:
                case VertexAttributeType::Int3:
                case VertexAttributeType::Int4:
                case VertexAttributeType::Bool:
                {
					glCall(glEnableVertexAttribArray(m_VertexBufferAttributeIndex));
                    glVertexAttribIPointer(m_VertexBufferAttributeIndex,
                                           element.GetComponentCount(),
                                           ShaderDataTypeToOpenGLBaseType(element.Type),
                                           layout.GetStride(),
                                           (const void*)element.Offset);
                    m_VertexBufferAttributeIndex++;
                    break;
                }
                /// Cast to float type matrix,  (glVertexAttribPointer)
                case VertexAttributeType::Mat3:
                case VertexAttributeType::Mat4:
                {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 0; i < count; i++)
                    {
						glCall(glEnableVertexAttribArray(m_VertexBufferAttributeIndex));
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
                    DYE_LOG("Unknown VertexAttributeType!");
                    DYE_ASSERT(false);
                    break;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        glBindVertexArray(m_ID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
}