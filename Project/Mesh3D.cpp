//
// Created by theom on 06.04.2024.
//

#define _USE_MATH_DEFINES

#include "Mesh3D.h"
#include "CommandBuffer.h"
#include "DescriptorPool.h"
#include "Singleton.h"
#include "DeltaTime.h"
#include "vulkanbase/VulkanBase.h"
#include <Math.h>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"


void Mesh3D::UploadBuffer(const CommandPool& pool, VkQueue const& graphicsQueue)
{
    m_VertexBuffer.Upload(graphicsQueue, pool);
    m_IndexBuffer.Upload(graphicsQueue, pool);

}

void Mesh3D::Destroy()
{
    m_VertexBuffer.Destroy();
    m_IndexBuffer.Destroy();
    m_DescriptorPool.Destroy();
}

void
Mesh3D::Draw(const CommandBuffer& buffer, uint32_t currentFrame, const GraphicsPipeline& pipeline, const VertexUBO& cam)
{
    m_RotationMatrix = glm::rotate(m_RotationMatrix,
                                   dae::Singleton<DeltaTime>::GetInstance().GetDeltaTime() * glm::radians(90.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
    m_Ubo.model = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
    //m_Ubo.view  = cam.view;
    //m_Ubo.proj  = cam.proj;
    m_VertexBuffer.InitAsVertexBuffer(buffer);
    m_IndexBuffer.InitAsIndexBuffer(buffer);
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, m_Ubo);
    m_DescriptorPool.BindDescriptorSets(buffer, currentFrame, pipeline);
    vkCmdDrawIndexed(buffer.GetCommandBuffer(), static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh3D::AddVertex(const glm::vec3& position, const glm::vec3& color)
{
    AddVertex(Vertex3D{position, color});
}

Mesh3D::Mesh3D()
{
    m_DescriptorPool.Initialize();
    m_Ubo.model = glm::mat4(1.f);
    m_Ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));
    m_Ubo.proj  = glm::perspective(glm::radians(45.0f),
                                   VulkanBase::swapChainExtent.width /
                                   static_cast<float>( VulkanBase::swapChainExtent.height), 0.1f,
                                   10.0f);
}

void Mesh3D::InitCube(const glm::vec3& pos, float length)
{

    const glm::vec3 fwd{0.f, 0.f, 1.f};
    const glm::vec3 up{0.f, 1.f, 0.f};
    const glm::vec3 right{glm::vec3{1.f, 0.f, 0.f}};

    const Vertex3D bottomLeftBack{pos, {1, 1, 1}};
    const Vertex3D bottomRightBack{pos + length * right, {1, 1, 1}};
    const Vertex3D topLeftBack{pos + length * up, {1, 1, 1}};
    const Vertex3D topRightBack{bottomRightBack.m_Pos + length * up, {1, 1, 1}};
    const Vertex3D bottomLeftFront{pos - length * fwd, {1, 1, 1}};
    const Vertex3D bottomRightFront{bottomRightBack.m_Pos - length * fwd, {1, 1, 1}};
    const Vertex3D topRightFront{topRightBack.m_Pos - length * fwd, {1, 1, 1}};
    const Vertex3D topLeftFront{topLeftBack.m_Pos - length * fwd, {1, 1, 1}};

    CreateFace(bottomLeftBack, topLeftBack, topRightBack, bottomRightBack, false);
    CreateFace(bottomLeftFront, topLeftFront, topRightFront, bottomRightFront, true);
    CreateFace(bottomLeftBack, topLeftBack, topLeftFront, bottomLeftFront, false);
    CreateFace(bottomRightBack, topRightBack, topRightFront, bottomRightFront, true);
    CreateFace(topLeftBack, topLeftFront, topRightFront, topRightBack, true);
    CreateFace(bottomLeftBack, bottomLeftFront, bottomRightFront, bottomRightBack, true);

    m_VertexBuffer.MapMemory(m_Vertices.size() * sizeof(Vertex3D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
}

void
Mesh3D::CreateFace(const Vertex3D& btmLeft, const Vertex3D& topLeft, const Vertex3D& topRight, const Vertex3D& btmRight,
                   bool isClockwise)
{
    if (isClockwise)
    {
        AddVertex(btmLeft);
        AddVertex(topLeft);
        AddVertex(topRight);
        AddVertex(btmLeft);
        AddVertex(topRight);
        AddVertex(btmRight);
    }
    else
    {
        AddVertex(btmLeft);
        AddVertex(topRight);
        AddVertex(topLeft);
        AddVertex(btmLeft);
        AddVertex(btmRight);
        AddVertex(topRight);
    }

}

void Mesh3D::AddVertex(const Vertex3D& vert)
{
    m_Vertices.emplace_back(vert);
    m_Indices.emplace_back(static_cast<uint32_t >(m_Indices.size()));
}

void Mesh3D::LoadModel(const std::string& filePath)
{
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str()))
    {
        throw std::runtime_error(err);
    }
    for (const auto& shape: shapes)
    {
        for (const auto& index: shape.mesh.indices)
        {
            Vertex3D vertex{};


            vertex.m_Pos =
                    {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                    };
            vertex.m_Color = {1.f, 1.f, 1.f};
            m_Vertices.push_back(vertex);
            m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));
        }
    }
    m_VertexBuffer.MapMemory(m_Vertices.size() * sizeof(Vertex3D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
}

void Mesh3D::Translate(const glm::vec3& direction)
{
    m_TranslationMatrix = glm::translate(m_TranslationMatrix, direction);
}
