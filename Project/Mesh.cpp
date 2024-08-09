//
// Created by theom on 06.04.2024.
//

#include "Mesh.h"
#include "CommandBuffer.h"
#include "DescriptorPool.h"

#define _USE_MATH_DEFINES

#include <Math.h>

Mesh::Mesh(const glm::vec2& center, float radius, int segments)
{
    Vertex2D centerVertex           = {center, {1, 1, 1}};
    float    degreesBetweenSegments = 360.f / static_cast<float>(segments);
    float    currentDegrees{0};
    Vertex2D lastVert{{
                              center.x + (radius * static_cast<float>(cos(currentDegrees * (M_PI / 180)))),
                                 center.y + (radius * static_cast<float>(sin(currentDegrees * (M_PI / 180))))
                      },
                      {       1, 1, 1}};

    for (int i = 0; i < segments; ++i)
    {

        currentDegrees += degreesBetweenSegments;
        double angleInRads{currentDegrees * (M_PI / 180)};
        auto   currentVert = Vertex2D{{
                                              center.x + radius * static_cast<float>(cos(angleInRads)),
                                                 center.y + radius * static_cast<float>(sin(angleInRads))
                                      },
                                      {       1, 1, 1}};
        AddVertex(currentVert.m_Pos, currentVert.m_Color);
        AddVertex(centerVertex.m_Pos, centerVertex.m_Color);
        AddVertex(lastVert.m_Pos, lastVert.m_Color);
        lastVert = currentVert;
    }
    m_VertexBuffer.MapMemory(m_Vertices.size()*sizeof(Vertex2D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size()*sizeof(uint32_t), m_Indices.data());
}

Mesh::Mesh(const glm::vec2& bottomleft, float width, float height)
{
    AddVertex({bottomleft.x, bottomleft.y}, {1, 0, 0});
    AddVertex({bottomleft.x + width, bottomleft.y}, {0, 0, 1});
    AddVertex({bottomleft.x, bottomleft.y + height}, {0, 1, 0});

    AddVertex({bottomleft.x, bottomleft.y + height}, {0, 1, 0});
    AddVertex({bottomleft.x + width, bottomleft.y}, {0, 0, 1});
    AddVertex({bottomleft.x + width, bottomleft.y + height}, {1, 0, 0});
    m_VertexBuffer.MapMemory(m_Vertices.size() * sizeof(Vertex2D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size() * sizeof(uint32_t ), m_Indices.data());
}

void Mesh::UploadBuffer(const CommandPool& pool, VkQueue const& graphicsQueue)
{
    m_VertexBuffer.Upload(graphicsQueue, pool);
    m_IndexBuffer.Upload(graphicsQueue, pool);
}

void Mesh::Destroy()
{
    m_VertexBuffer.Destroy();
    m_IndexBuffer.Destroy();
}

void Mesh::Draw(const CommandBuffer& buffer, uint32_t currentFrame, const DescriptorPool& descriptorPool,
                const GraphicsPipeline& pipeline)
{
    m_VertexBuffer.InitAsVertexBuffer(buffer);
    m_IndexBuffer.InitAsIndexBuffer(buffer);
    descriptorPool.BindDescriptorSets(buffer, currentFrame, pipeline);
    vkCmdDrawIndexed(buffer.GetCommandBuffer(), static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh::AddVertex(const glm::vec2& position, const glm::vec3& color)
{
    Vertex2D vert{position, color};
    m_Vertices.emplace_back(vert);
    m_Indices.emplace_back(static_cast<int>(m_Indices.size()));
}
