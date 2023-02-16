#include "Scene/ImageRenderer.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Graphics/WindowBase.h"
#include "Util/Logger.h"
#include "Graphics/OpenGL.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Texture.h"

#include "Event/Event.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <utility>

#ifdef DYE_DEBUG
#include <imgui.h>
#endif

namespace DYE
{
    void ImageRenderer::OnUpdate()
    {
    }

    void ImageRenderer::SetTexture(std::shared_ptr<Texture2D> texture, bool setDimension)
    {
        m_Texture = std::move(texture);
        if (setDimension)
        {
            SetDimension(m_Texture->GetWidth(), m_Texture->GetHeight());
        }
    }

    ImageRendererUpdater::ImageRendererUpdater(ComponentTypeID typeID, WindowBase* window) : ComponentUpdaterBase(typeID), m_pWindow(window)
    {
    }

    void ImageRendererUpdater::Init()
    {
        DYE_LOG("Init Image Renderer");

        /// Create vertices [position, texCoord]
        float positions[4 * 4] = {
                -0.5f, -0.5f, 0.0f, 0.0f,
                0.5f, -0.5f, 1.0f, 0.0f,

                0.5f, 0.5f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 1.0f
        };

        // Index Buffer
        std::uint32_t indices[] = {
                0, 1, 2,
                2, 3, 0
        };

        m_QuadVertexArray = VertexArray::Create();

        auto vB = VertexBuffer::Create(positions, sizeof(positions));
        VertexLayout layout
		{
			VertexAttribute(VertexAttributeType::Float2, "position", false),
			VertexAttribute(VertexAttributeType::Float2, "texCoord", false),
		};
        vB->SetLayout(layout);
        m_QuadVertexArray->AddVertexBuffer(vB);

        auto iB = IndexBuffer::Create(indices, sizeof(indices) / sizeof(std::uint32_t));
        m_QuadVertexArray->SetIndexBuffer(iB);

        /// Create default shader program
        m_DefaultShaderProgram = ShaderProgram::CreateFromFile("Image", "assets/default/Image.shader");

        /// Create default white texture
        m_DefaultTexture2D = Texture2D::Create(glm::vec4 {1, 1, 1, 1});

        m_DefaultShaderProgram->Unbind();
        m_QuadVertexArray->Unbind();
        vB->Unbind();
        iB->Unbind();
    }

    void ImageRendererUpdater::UpdateComponents()
    {

    }

    void ImageRendererUpdater::FixedUpdateComponents()
    {

    }

    void ImageRendererUpdater::RemoveComponentsOfEntity(uint32_t entityID)
    {
        m_CachedImageRenderers.erase(std::remove_if(m_CachedImageRenderers.begin(), m_CachedImageRenderers.end(),
                                                    [entityID](const auto& image)
                                                    {
                                                        return image->GetEntityPtr()->GetID() == entityID;
                                                    }), m_CachedImageRenderers.end());
    }

    void ImageRendererUpdater::attachEntityWithComponent(const std::weak_ptr<GameObject> &entity,
                                                         const std::shared_ptr<ComponentBase> &component)
    {
        const auto imageRenderer = std::static_pointer_cast<ImageRenderer>(component);
        imageRenderer->m_pUpdater = this;

        MarkDirty();
        m_CachedImageRenderers.push_back(imageRenderer);
    }

    void ImageRendererUpdater::RenderImages()
    {
        if (m_IsDirty)
        {
            /// Sort first if it's marked as dirty
            sortCachedImageRenderers();
            m_IsDirty = false;
        }

        /// Draw
        for (const auto& image : m_CachedImageRenderers)
        {
            if (!image->GetEntityPtr()->IsActive())
            {
                continue;
            }

            if (!image->GetIsEnabled())
            {
                continue;
            }

            ///
            auto transform = image->GetEntityPtr()->GetTransform();
            auto worldPos = transform->GetLocalPosition();
            auto worldScale = transform->GetLocalScale();

            auto windowWidth = (float) m_pWindow->GetWidth();
            auto windowHeight = (float) m_pWindow->GetHeight();
            auto imageWidth = (float) image->GetWidth();
            auto imageHeight = (float) image->GetHeight();

            worldPos.y = windowHeight - worldPos.y;

            glm::vec2 position{(worldPos.x - windowWidth / 2.0f) / windowWidth * 2.0f,
                               (worldPos.y - windowHeight / 2.0f) / windowHeight * 2.0f};

            float dimensionScaleX = imageWidth / (windowWidth * 0.5f);
            float dimensionScaleY = imageHeight / (windowHeight * 0.5f);
            glm::vec2 scale{dimensionScaleX * worldScale.x, dimensionScaleY * worldScale.y};

            glm::mat4 transformMatrix =
                    glm::translate(glm::vec3(position.x, position.y, 0)) *
                    glm::scale(glm::vec3(scale.x, scale.y, 1));

            m_DefaultShaderProgram->Use();
			{
				if (image->m_Texture)
				{
					// Bind the texture to the first texture slot.
					image->m_Texture->Bind(0);
				}
				else
				{
					// Bind the texture to the first texture slot.
					m_DefaultTexture2D->Bind(0);
				}

				auto colorUniformLocation = glGetUniformLocation(m_DefaultShaderProgram->GetID(), "_Color");
				glCall(glUniform4f(colorUniformLocation, image->m_Color.r, image->m_Color.g, image->m_Color.b, image->m_Color.a));

				auto transformMatUniformLocation = glGetUniformLocation(m_DefaultShaderProgram->GetID(), "_TransformMatrix");
				glCall(glUniformMatrix4fv(transformMatUniformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix)););
			}

            ///
			RenderCommand::GetInstance().DrawIndexedNow(*m_QuadVertexArray.get());

            m_DefaultShaderProgram->Unbind();
        }
    }

    void ImageRendererUpdater::PushSortingLayer(const std::string &layerName)
    {
        if (std::find(m_SortingLayers.begin(), m_SortingLayers.end(), layerName) != std::end(m_SortingLayers))
        {
            DYE_LOG("A sorting layer with the name <%s> already exists!", layerName.c_str());
        }
        else
        {
            m_SortingLayers.push_back(layerName);
        }
    }

	std::uint32_t ImageRendererUpdater::LayerNameToLayerID(const std::string &layerName)
    {
        std::uint32_t id = 0;
        for (const auto& name : m_SortingLayers)
        {
            if (name == layerName)
            {
                return id;
            }
        }

        return 0;
    }

    std::string ImageRendererUpdater::LayerIDToLayerName(std::uint32_t id)
    {
        /// Out of range, return the first layer name
        if (id >= m_SortingLayers.size())
        {
            return "<unknown layer>";
        }

        return m_SortingLayers[id];
    }


    void ImageRendererUpdater::sortCachedImageRenderers()
	{
		struct
		{
			bool operator()(const std::shared_ptr<ImageRenderer> &a, std::shared_ptr<ImageRenderer> b) const
			{
				if (a->m_SortingLayerID < b->m_SortingLayerID)
				{
					return true;
				}
				else if (a->m_SortingLayerID > b->m_SortingLayerID)
				{
					return false;
				}
				else
				{
					return a->m_SortingOrder < b->m_SortingOrder;
				}
			}
		} customSort;
		std::sort(m_CachedImageRenderers.begin(), m_CachedImageRenderers.end(), customSort);
	}

#ifdef DYE_DEBUG
    void ImageRenderer::onComponentDebugWindowGUI(float width, float height)
    {
        static bool isColorPickerOn = false;

        ComponentBase::onComponentDebugWindowGUI(width, height);

        /// Color Picker
        {
            ImGui::Text("Color");
            ImVec4 colorIm{m_Color.r, m_Color.g, m_Color.b, m_Color.a};
            if (ImGui::ColorButton("Color", colorIm))
            {
                isColorPickerOn = true;
            }

            if (isColorPickerOn)
            {
                if (ImGui::Begin("Image Renderer: Color Picker Window", &isColorPickerOn))
                {
                    ImGui::ColorPicker4("##colorPicker", (float *) &m_Color);
                }
                ImGui::End();
            }
        }

        /// Texture Info
        {
            ImGui::Text("Texture");
            if (m_Texture)
            {
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", m_Texture->GetPath().c_str());
                ImGui::SameLine();

                if (ImGui::Button("Set Native Size"))
                {
                    SetDimension(m_Texture->GetWidth(), m_Texture->GetHeight());
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0.4, 0.4, 1), "<empty>");
            }
        }

        /// Dimension
        {
            ImGui::PushItemWidth(width * 0.5f / 3);

            ImGui::Text("Dimension");
            ImGui::DragInt("width", (int *) &m_Width, 1, 0, INT32_MAX);
            ImGui::SameLine();
            ImGui::DragInt("height", (int *) &m_Height, 1, 0, INT32_MAX);

            ImGui::PopItemWidth();
        }

        ImGui::Separator();

        /// Sorting Layer
        {
            ImGui::Text("Sorting Layer");
            const auto &layers = m_pUpdater->GetSortingLayers();

            char previewLayerName[128];
            sprintf(previewLayerName, "%2d: %s", m_SortingLayerID,
                    m_pUpdater->LayerIDToLayerName(m_SortingLayerID).c_str());

            bool isDropdownOpen = ImGui::BeginCombo("##sortingLayer", previewLayerName);
            if (isDropdownOpen)
            {
                std::uint32_t index = 0;
                char itemLayerName[128];
                for (const auto &layerName : layers)
                {
                    sprintf(itemLayerName, "%2d: %s##%d", index, m_pUpdater->LayerIDToLayerName(index).c_str(),
                            GetEntityPtr()->GetID());

                    bool isSelected = (index == m_SortingLayerID);
                    if (ImGui::Selectable(itemLayerName, isSelected))
                    {
                        if (index != m_SortingLayerID)
                        {
                            m_pUpdater->MarkDirty();
                            m_SortingLayerID = index;
                        }
                    }

                    if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

                    index++;
                }
                ImGui::EndCombo();
            }

            ImGui::Text("Sorting Order In Layer");
            int sortingOrder = m_SortingOrder;
            ImGui::DragInt("##sortingOrder", (int *) &sortingOrder, 0.1f, 0, 0);

            ImGui::SameLine();
            if (ImGui::Button("<"))
                sortingOrder--;
            ImGui::SameLine();
            if (ImGui::Button(">"))
                sortingOrder++;

            if (sortingOrder != m_SortingOrder)
            {
                m_pUpdater->MarkDirty();
                m_SortingOrder = sortingOrder;
            }
        }
    }

    void ImageRendererUpdater::onUpdaterDebugWindowGUI(float width, float height)
    {
        ComponentUpdaterBase::onUpdaterDebugWindowGUI(width, height);

        ImVec4 enabledTextColor {1, 1, 1, 1 };
        ImVec4 disabledTextColor {0.5, 0.5, 0.5, 1 };

        ImGui::Text("Rendering Order");
        ImGui::BeginChild("cached image renderers", ImVec2(width - 20, 0), true);
        for (const auto& image : m_CachedImageRenderers)
        {
            char imageRendererLabel[128];
            sprintf(imageRendererLabel, "[Layer: %s, Order: %d] %s##", LayerIDToLayerName(image->m_SortingLayerID).c_str(),
                    image->m_SortingOrder, image->GetEntityPtr()->GetName().c_str());

            ImGui::PushStyleColor(ImGuiCol_Text, image->GetIsEnabled() ? enabledTextColor : disabledTextColor);
            ImGui::Text("%s", imageRendererLabel);
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
    }

#endif
}
