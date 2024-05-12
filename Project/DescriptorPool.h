//
// Created by theom on 11.04.2024.
//

#ifndef VULKANLAB01_DESCRIPTORPOOL_H
#define VULKANLAB01_DESCRIPTORPOOL_H

#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "CommandBuffer.h"
#include "GraphicsPipeline.h"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

class DescriptorPool
{
public:
    DescriptorPool() = default;
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) = delete;
    ~DescriptorPool() = default;
    void Destroy();
    void UpdateUniformBuffer(uint32_t currentImage, const VertexUBO& ubo);
    void BindDescriptorSets(const CommandBuffer& cmdBuffer, uint32_t currentFrame,
                            const GraphicsPipeline& pipeline) const;
    void Initialize();
private:
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    std::vector<VkBuffer>        m_UniformBuffers;
    std::vector<VkDeviceMemory>  m_UniformBufferMemories;
    std::vector<void*>           m_UniformBuffersMapped;
    VkDescriptorPool             m_DescriptorPool;
    std::vector<VkDescriptorSet> m_DescriptorSets;
};


#endif //VULKANLAB01_DESCRIPTORPOOL_H
