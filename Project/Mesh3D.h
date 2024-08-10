//
// Created by theom on 06.04.2024.
//

#ifndef VULKANLAB01_MESH3D_H
#define VULKANLAB01_MESH3D_H


#include <unordered_map>
#include "DataBuffer.h"
#include "CommandBuffer.h"
#include "Vertex.h"
#include "DescriptorPool.h"
#include "Textures/Texture.h."

class Mesh3D
{
public:
    Mesh3D();
    Mesh3D(const Mesh3D&) = delete;
    Mesh3D(Mesh3D&&) = delete;
    Mesh3D& operator=(const Mesh3D&) = delete;
    Mesh3D& operator=(Mesh3D&&) = delete;
    ~Mesh3D() = default;

    void UploadBuffer(const CommandPool& pool, const VkQueue& graphicsQueue);
    void Destroy();
    void Draw(const CommandBuffer& buffer, uint32_t currentFrame, const GraphicsPipeline& pipeline, const VertexUBO& cam);
    void Update(VertexUBO &ubo, uint32_t currentFrame);
    void InitCube(const glm::vec3& pos, float length);
    void InitPlane(const glm::vec3& pos, float length);
    void LoadModel(const std::string& filePath);
    void LoadTexture(const CommandPool &pool, VkQueue graphicsQueue, const std::string &albedoPath = "Models/white.png",
                     const std::string &normalPath = "Models/white.png", const std::string &roughnessPath = "Models/white.png", const std::string &specularPath = "Models/white.png");
    void InitDescriptor();
    void Translate(const glm::vec3& direction);
    inline static FragPushConstants constants {glm::vec3{},1,3};
private:
    void AddVertex(const glm::vec3& position, const glm::vec3& color);
    void AddVertex(const Vertex3D& vert);
    void
    CreateFace(const Vertex3D& topLeft, const Vertex3D& btmLeft, const Vertex3D& topRight, const Vertex3D& btmRight,
               bool isClockwise);
    std::string EmptyStringToDefaultTexture(const std::string& path);
    DataBuffer            m_VertexBuffer{
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    DataBuffer            m_IndexBuffer{
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    DescriptorPool        m_DescriptorPool{};
    std::vector<Vertex3D> m_Vertices;
    std::vector<uint32_t> m_Indices;
    VertexUBO             m_Ubo{};
    glm::mat4             m_TranslationMatrix{glm::mat4(1.f)};
    glm::mat4             m_RotationMatrix{glm::mat4(1.f)};
    glm::mat4             m_ScaleMatrix{glm::mat4(1.f)};
    std::unique_ptr<dae::Texture> m_pAlbedoMap{nullptr};
    std::unique_ptr<dae::Texture> m_pNormalMap{nullptr};
    std::unique_ptr<dae::Texture> m_pRoughnessMap{nullptr};
    std::unique_ptr<dae::Texture> m_pSpecularMap{nullptr};

};


#endif //VULKANLAB01_MESH_H
