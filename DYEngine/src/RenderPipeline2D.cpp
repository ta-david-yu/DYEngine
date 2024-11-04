#include "Graphics/RenderPipeline2D.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Util/Algorithm.h"

#include <array>

namespace DYE
{
    RenderPipeline2D::RenderPipeline2D()
    {
        // Create vertices [position, color, texCoord]
        //				   [       3,     4,        2] = 9 elements per vertex
        float vertices[9 * 4] =
            {
                -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

                0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
            };

        std::array<std::uint32_t, 6> indices =
            {
                0, 1, 2,
                2, 3, 0,
            };

        m_DefaultSpriteVAO = VertexArray::Create();

        {
            auto vertexBufferObject = VertexBuffer::Create(vertices, sizeof(vertices));
            VertexLayout const vertexLayout
                {
                    VertexAttribute(VertexAttributeType::Float3, "position", false),
                    VertexAttribute(VertexAttributeType::Float4, "color", false),
                    VertexAttribute(VertexAttributeType::Float2, "texCoord", false),
                };

            vertexBufferObject->SetLayout(vertexLayout);
            m_DefaultSpriteVAO->AddVertexBuffer(vertexBufferObject);

            auto indexBufferObject = IndexBuffer::Create(indices.data(), indices.size());
            m_DefaultSpriteVAO->SetIndexBuffer(indexBufferObject);
        }

        auto spriteShader = ShaderProgram::CreateFromFile("Shader_SpriteDefault", "assets\\default\\SpriteDefault.shader");
        spriteShader->Use();

        m_DefaultSpriteMaterial = Material::CreateFromShader("Material_Sprite", spriteShader);

    }

    void RenderPipeline2D::Submit
        (
            const std::shared_ptr<VertexArray> &vertexArray,
            const std::shared_ptr<Material> &material,
            glm::mat4 objectToWorldMatrix,
            MaterialPropertyBlock materialPropertyBlock
        )
    {
        m_Submissions.push_back
            (
                RenderSubmission2D
                    {
                        .VertexArray = vertexArray,
                        .Material = material,
                        .ObjectToWorldMatrix = objectToWorldMatrix,
                        .MaterialPropertyBlock = std::move(materialPropertyBlock)
                    }
            );

        // TODO: we want to use insertion sort to sort all the submissions everytime a new sprite is submitted
        /*
        Algorithm::InsertionSort
        (
            m_Submissions.begin(),
            m_Submissions.end(),
            [](auto const& submissionA, auto const& submissionB)
            {
                return true;
            }
        );*/
    }

    void RenderPipeline2D::SubmitSprite(const std::shared_ptr<Texture2D> &texture, glm::vec4 color, glm::mat4 objectToWorldMatrix)
    {
        // Scale the matrix based on sprite pixels per unit.
        objectToWorldMatrix = glm::scale(objectToWorldMatrix, texture->GetScaleFromTextureDimensions());

        MaterialPropertyBlock materialPropertyBlock;
        materialPropertyBlock.SetTexture("_MainTex", texture);
        materialPropertyBlock.SetFloat4("_MainTex_TilingOffset", {1, 1, 0, 0});
        materialPropertyBlock.SetFloat4("_Color", color);

        m_Submissions.push_back
            (
                RenderSubmission2D
                    {
                        .VertexArray = m_DefaultSpriteVAO,
                        .Material = m_DefaultSpriteMaterial,
                        .ObjectToWorldMatrix = objectToWorldMatrix,
                        .MaterialPropertyBlock = std::move(materialPropertyBlock)
                    }
            );
    }

    void RenderPipeline2D::SubmitTiledSprite(const std::shared_ptr<Texture2D> &texture, glm::vec4 tilingOffset,
                                             glm::vec4 color, glm::mat4 objectToWorldMatrix)
    {
        // Scale the matrix based on sprite pixels per unit.
        objectToWorldMatrix = glm::scale(objectToWorldMatrix, texture->GetScaleFromTextureDimensions());

        MaterialPropertyBlock materialPropertyBlock;
        materialPropertyBlock.SetTexture("_MainTex", texture);
        materialPropertyBlock.SetFloat4("_MainTex_TilingOffset", tilingOffset);
        materialPropertyBlock.SetFloat4("_Color", color);

        m_Submissions.push_back
            (
                RenderSubmission2D
                    {
                        .VertexArray = m_DefaultSpriteVAO,
                        .Material = m_DefaultSpriteMaterial,
                        .ObjectToWorldMatrix = objectToWorldMatrix,
                        .MaterialPropertyBlock = std::move(materialPropertyBlock)
                    }
            );
    }

    void RenderPipeline2D::onPreRender()
    {
        // Do nothing.
    }

    void RenderPipeline2D::renderCamera(const Camera &camera)
    {
        // Sort the submission (render queue, sorting layer, camera distance etc).
        // TODO: we want to use insertion sort instead cuz the vector will already be kinda sorted in every Submit call
        // TODO: we can also further decode the sorting criteria into an integer and use radix sort as well (see: https://youtu.be/4BISSS3MCNA)
        glm::mat4 const viewMatrix = camera.ViewMatrix;
        std::stable_sort
            (
                m_Submissions.begin(),
                m_Submissions.end(),
                [viewMatrix](RenderSubmission2D const &submissionA, RenderSubmission2D const &submissionB)
                {
                    auto modelViewMatrixA = viewMatrix * submissionA.ObjectToWorldMatrix;
                    auto modelViewMatrixB = viewMatrix * submissionB.ObjectToWorldMatrix;
                    glm::vec4 const viewPositionA = modelViewMatrixA * glm::vec4 {0, 0, 0, 1};
                    glm::vec4 const viewPositionB = modelViewMatrixB * glm::vec4 {0, 0, 0, 1};
                    return viewPositionA.z < viewPositionB.z;
                }
            );

        // Execute draw-calls.
        for (auto const &submission: m_Submissions)
        {
            RenderCommand::GetInstance().DrawIndexedNow
                (
                    RenderParameters {.Camera = camera, .Material = submission.Material, .PropertyBlock = submission.MaterialPropertyBlock},
                    *submission.VertexArray,
                    submission.ObjectToWorldMatrix
                );
        }
    }

    void RenderPipeline2D::onPostRender()
    {
        // Clean up the submissions.
        m_Submissions.clear();
    }
}