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
    std::array<VkDescriptorPoolSize,2> poolSizes{};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
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

        VkDescriptorImageInfo albedoInfo{};
        albedoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        albedoInfo.imageView = m_AlbedoImageView;
        albedoInfo.sampler = m_AlbedoSampler;

        VkDescriptorImageInfo normalInfo{};
        normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        normalInfo.imageView = m_NormalImageView;
        normalInfo.sampler = m_NormalSampler;

        VkDescriptorImageInfo roughnessInfo{};
        roughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        roughnessInfo.imageView = m_RoughnessImageView;
        roughnessInfo.sampler = m_RoughnessSampler;

        VkDescriptorImageInfo specularInfo{};
        specularInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        specularInfo.imageView = m_SpecularImageView;
        specularInfo.sampler = m_SpecularSampler;


        std::array<VkWriteDescriptorSet,5> descriptorWrite{};
        descriptorWrite[0].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet           = m_DescriptorSets[i];
        descriptorWrite[0].dstBinding       = 0;
        descriptorWrite[0].dstArrayElement  = 0;
        descriptorWrite[0].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount  = 1;
        descriptorWrite[0].pBufferInfo      = &bufferInfo;
        descriptorWrite[0].pImageInfo       = nullptr;
        descriptorWrite[0].pTexelBufferView = nullptr;

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].dstSet = m_DescriptorSets[i];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = &albedoInfo;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].dstSet = m_DescriptorSets[i];
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = &normalInfo;

        descriptorWrite[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[3].dstSet = m_DescriptorSets[i];
        descriptorWrite[3].dstBinding = 3;
        descriptorWrite[3].dstArrayElement = 0;
        descriptorWrite[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[3].descriptorCount = 1;
        descriptorWrite[3].pImageInfo = &roughnessInfo;

        descriptorWrite[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[4].dstSet = m_DescriptorSets[i];
        descriptorWrite[4].dstBinding = 4;
        descriptorWrite[4].dstArrayElement = 0;
        descriptorWrite[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[4].descriptorCount = 1;
        descriptorWrite[4].pImageInfo = &specularInfo;

        vkUpdateDescriptorSets(VulkanBase::device,
                               static_cast<uint32_t>(descriptorWrite.size()),
                               descriptorWrite.data(),
                               0,
                               nullptr
                               );


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

void DescriptorPool::InitImageView(VkImageView albedoView, VkImageView normalView, VkImageView roughnessView,
                                   VkImageView specularView) {
    m_AlbedoImageView = albedoView;
    m_NormalImageView = normalView;
    m_RoughnessImageView = roughnessView;
    m_SpecularImageView = specularView;
}

void DescriptorPool::InitSampler(VkSampler albedoSampler, VkSampler normalSampler, VkSampler roughnessSampler,
                                 VkSampler specularSampler) {
    m_AlbedoSampler = albedoSampler;
    m_NormalSampler = normalSampler;
    m_RoughnessSampler = roughnessSampler;
    m_SpecularSampler = specularSampler;
}
