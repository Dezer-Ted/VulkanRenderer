//
// Created by theom on 06.04.2024.
//

#ifndef VULKANLAB01_MESH_H
#define VULKANLAB01_MESH_H


#include <unordered_map>
#include "DataBuffer.h"
#include "CommandBuffer.h"
#include "Vertex.h"
#include "DescriptorPool.h"

class Mesh
{
public:
    //Init a cicular 2D Mesh
    explicit Mesh(const glm::vec2& center, float radius, int segments);
    //Init a Rectangular 2D Mesh
    explicit Mesh(const glm::vec2& bottomleft, float width, float height);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    ~Mesh() = default;

    void UploadBuffer(const CommandPool& pool, const VkQueue& graphicsQueue);
    void Destroy();
    void Draw(const CommandBuffer& buffer, uint32_t currentFrame, const DescriptorPool& descriptorPool,
              const GraphicsPipeline& pipeline);

private:
    void AddVertex(const glm::vec2& position, const glm::vec3& color);
    DataBuffer            m_VertexBuffer{
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    DataBuffer            m_IndexBuffer{
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    std::vector<Vertex2D> m_Vertices;
    std::vector<uint32_t> m_Indices;
};


#endif //VULKANLAB01_MESH_H
