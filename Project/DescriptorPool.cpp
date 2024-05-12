//
// Created by theom on 11.04.2024.
//

#include "DescriptorPool.h"
#include "DataBuffer.h"
#include "vulkanbase/VulkanBase.h"
//#include <chrono>
#include "ShaderModule.h"


void DescriptorPool::CreateUniformBuffers()
{
    VkDeviceSize buffersize = sizeof(VertexUBO);
    m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_UniformBufferMemories.resize(MAX_FRAMES_IN_FLIGHT);
    m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        DataBuffer::CreateBuffer(buffersize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 m_UniformBuffers[i], m_UniformBufferMemories[i]);
        vkMapMemory(VulkanBase::device, m_UniformBufferMemories[i], 0, buffersize, 0, &m_UniformBuffersMapped[i]);
    }
}

void DescriptorPool::Destroy()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(VulkanBase::device, m_UniformBuffers[i], nullptr);
        vkFreeMemory(VulkanBase::device, m_UniformBufferMemories[i], nullptr);
    }
    vkDestroyDescriptorPool(VulkanBase::device, m_DescriptorPool, nullptr);
}

void DescriptorPool::UpdateUniformBuffer(uint32_t currentImage, const VertexUBO& ubo)
{
    memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void DescriptorPool::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes    = &poolSize;

    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    if (vkCreateDescriptorPool(VulkanBase::device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create Descriptor pool");
    }

}

void DescriptorPool::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, ShaderModule::m_DescriptorSetLayout);
    VkDescriptorSetAllocateInfo        allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts.data();
    m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(VulkanBase::device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocated descriptor sets");
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(VertexUBO);
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet           = m_DescriptorSets[i];
        descriptorWrite.dstBinding       = 0;
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        vkUpdateDescriptorSets(VulkanBase::device, 1, &descriptorWrite, 0, nullptr);

    }

}

void DescriptorPool::BindDescriptorSets(const CommandBuffer& cmdBuffer, uint32_t currentFrame,
                                        const GraphicsPipeline& pipeline) const
{
    vkCmdBindDescriptorSets(cmdBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.GetLayout(), 0, 1, &m_DescriptorSets[currentFrame],
                            0, nullptr);
}

void DescriptorPool::Initialize()
{
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();

}
