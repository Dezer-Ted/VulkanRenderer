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
#include "nlohmann/json.hpp"
#include "tiny_obj_loader.h"


void Mesh3D::UploadBuffer(const CommandPool& pool, VkQueue const& graphicsQueue)
{
    m_VertexBuffer.Upload(graphicsQueue, pool);
    m_IndexBuffer.Upload(graphicsQueue, pool);

}

void Mesh3D::Destroy()
{
    m_pAlbedoMap->CleanUp();
    m_pRoughnessMap->CleanUp();
    m_pNormalMap->CleanUp();
    m_pSpecularMap->CleanUp();
    m_VertexBuffer.Destroy();
    m_IndexBuffer.Destroy();
    m_DescriptorPool.Destroy();
}

void
Mesh3D::Draw(const CommandBuffer& buffer, uint32_t currentFrame, const GraphicsPipeline& pipeline, const VertexUBO& cam)
{

    m_VertexBuffer.InitAsVertexBuffer(buffer);
    m_IndexBuffer.InitAsIndexBuffer(buffer);
    m_DescriptorPool.BindDescriptorSets(buffer, currentFrame, pipeline);

    vkCmdPushConstants(buffer.GetCommandBuffer(),
                       pipeline.GetLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,sizeof(FragPushConstants),
                       &constants);
    vkCmdDrawIndexed(buffer.GetCommandBuffer(),
                     static_cast<uint32_t>(m_Indices.size()),
                     1,
                     0,
                     0,
                     0);
}

void Mesh3D::AddVertex(const glm::vec3& position, const glm::vec3& color)
{
    AddVertex(Vertex3D{position, color});
}

Mesh3D::Mesh3D()
{
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
            vertex.m_TexCoord = {
                    attrib.texcoords[2*index.texcoord_index+0],
                    1.0f - attrib.texcoords[2*index.texcoord_index+1]
            };
            vertex.m_Normal = {
                    attrib.normals[3*index.normal_index + 0],
                    attrib.normals[3*index.normal_index + 1],
                    attrib.normals[3*index.normal_index + 2]
            };
            vertex.m_Color = {1.f, 1.f, 1.f};
            m_Vertices.push_back(vertex);
            m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));
        }
    }
    for(uint32_t i {0}; i < m_Indices.size(); i+=3) {
        uint32_t index0 = m_Indices[i];
        uint32_t index1 = m_Indices[size_t(i)+1];
        uint32_t index2 = m_Indices[size_t(i)+2];

        const glm::vec3& p0 = m_Vertices[index0].m_Pos;
        const glm::vec3& p1 = m_Vertices[index1].m_Pos;
        const glm::vec3& p2 = m_Vertices[index2].m_Pos;
        const glm::vec2& uv0 = m_Vertices[index0].m_TexCoord;
        const glm::vec2& uv1 = m_Vertices[index1].m_TexCoord;
        const glm::vec2& uv2 = m_Vertices[index2].m_TexCoord;

        const glm::vec3 edge0 = p1-p0;
        const glm::vec3 edge1 = p2-p0;
        const glm::vec2 diffX = glm::vec2{uv1.x-uv0.x,uv2.x-uv0.x};
        const glm::vec2 diffY = glm::vec2{uv1.y-uv0.y,uv2.y-uv0.y};
        float r = 1.f / (diffX.x * diffY.y - diffY.x * diffX.y);

        glm::vec3 tan = {1.f,1.f,1.f};

        if(abs(diffX.x * diffY.y - diffY.x * diffX.y) < 0.01f) {
             tan = (edge0*diffY.y-edge1*diffY.x)*r;
        }

        m_Vertices[index0].m_Tangent += tan;
        m_Vertices[index1].m_Tangent += tan;
        m_Vertices[index2].m_Tangent += tan;

    }
    for(auto& v : m_Vertices) {
        v.m_Tangent = glm::normalize( v.m_Tangent -  (glm::dot(v.m_Tangent,v.m_Normal)/glm::dot(v.m_Normal,v.m_Normal))* v.m_Normal);
    }
    m_VertexBuffer.MapMemory(m_Vertices.size() * sizeof(Vertex3D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
}

void Mesh3D::Translate(const glm::vec3& direction)
{
    m_TranslationMatrix = glm::translate(m_TranslationMatrix, direction);
}

void Mesh3D::Update(VertexUBO &ubo, uint32_t currentFrame) {
    m_RotationMatrix = glm::rotate(m_RotationMatrix,
                                   dae::Singleton<DeltaTime>::GetInstance().GetDeltaTime() * glm::radians(90.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.model = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, ubo);

}

void Mesh3D::LoadTexture(const CommandPool &pool, VkQueue graphicsQueue, const std::string &albedoPath,
                         const std::string &normalPath, const std::string &roughnessPath,
                         const std::string &specularPath) {

    m_pAlbedoMap = std::make_unique<dae::Texture>(EmptyStringToDefaultTexture(albedoPath), pool, graphicsQueue);
    m_pNormalMap = std::make_unique<dae::Texture>(EmptyStringToDefaultTexture(normalPath),pool,graphicsQueue);
    m_pRoughnessMap = std::make_unique<dae::Texture>(EmptyStringToDefaultTexture(roughnessPath),pool,graphicsQueue);
    m_pSpecularMap = std::make_unique<dae::Texture>(EmptyStringToDefaultTexture(specularPath), pool, graphicsQueue);

}

void Mesh3D::InitDescriptor() {
    m_DescriptorPool.InitImageView(
            m_pAlbedoMap->GetImageView(),
            m_pNormalMap->GetImageView(),
            m_pRoughnessMap->GetImageView(),
            m_pSpecularMap->GetImageView()
                                   );
    m_DescriptorPool.InitSampler(
            m_pAlbedoMap->GetSampler(),
            m_pNormalMap->GetSampler(),
            m_pRoughnessMap->GetSampler(),
           m_pSpecularMap->GetSampler()
            );
    m_DescriptorPool.Initialize();
}

void Mesh3D::InitPlane(const glm::vec3 &pos, float length) {
    Vertex3D bottomRight {pos,{1,1,1},{1.f,0.f}};
    Vertex3D bottomLeft {{pos.x+length,pos.y, pos.z},{1,1,1},{1.f,1.f}};
    Vertex3D topRight {{pos.x,pos.y+length, pos.z},{1,1,1},{0.f,0.f}};
    Vertex3D topLeft {{pos.x+length,pos.y+length, pos.z},{1,1,1},{0.f,1.f}};
    CreateFace(bottomLeft,topLeft,topRight,bottomRight,true);
    m_VertexBuffer.MapMemory(m_Vertices.size() * sizeof(Vertex3D), m_Vertices.data());
    m_IndexBuffer.MapMemory(m_Indices.size() * sizeof(uint32_t), m_Indices.data());
}

std::string Mesh3D::EmptyStringToDefaultTexture(const std::string &path) {
    return (path.empty()) ? "Models/white.png" : path;
}
