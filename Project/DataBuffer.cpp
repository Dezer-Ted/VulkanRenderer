//
// Created by theom on 05.04.2024.
//

#include "DataBuffer.h"
#include "vulkanbase/VulkanBase.h"

DataBuffer::DataBuffer(const VkBufferUsageFlags& usageFlags, const VkMemoryPropertyFlags& properties)
{
    m_UsageFlags = usageFlags;
    m_Properties = properties;
}

VkBuffer DataBuffer::GetVertexBuffer() const
{
    return m_Buffer;
}

void DataBuffer::CleanUp()
{
    vkDestroyBuffer(VulkanBase::device, m_Buffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_Memory, nullptr);
}

uint32_t DataBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(VulkanBase::physicalDevice, &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type");

}

void
DataBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer,
                         VkDeviceMemory& buffermemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanBase::device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex Buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanBase::device, buffer, &memRequirements);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, props);

    if (vkAllocateMemory(VulkanBase::device, &allocInfo, nullptr, &buffermemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    vkBindBufferMemory(VulkanBase::device, buffer, buffermemory, 0);

}

void DataBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool,
                            VkQueue graphicsQueue)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(VulkanBase::device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(VulkanBase::device, pool, 1, &commandBuffer);
}

void DataBuffer::MapMemory(VkDeviceSize size, void* data)
{
    m_Size = size;
    CreateBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 m_Properties, m_StagingBuffer,
                 m_StagingBufferMemory);
    void* targetData;
    vkMapMemory(VulkanBase::device, m_StagingBufferMemory, 0, m_Size, 0, &targetData);
    memcpy(targetData, data, static_cast<size_t>(m_Size));
    vkUnmapMemory(VulkanBase::device, m_StagingBufferMemory);

    CreateBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_UsageFlags,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_Memory);
}

void DataBuffer::Upload(VkQueue const& graphicsQueue, const CommandPool& pool)
{
    CopyBuffer(m_StagingBuffer, m_Buffer, m_Size, pool.GetCommandPool(), graphicsQueue);
    vkDestroyBuffer(VulkanBase::device, m_StagingBuffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_StagingBufferMemory, nullptr);
}

void DataBuffer::Destroy()
{
    vkDestroyBuffer(VulkanBase::device, m_Buffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_Memory, nullptr);
}

void DataBuffer::InitAsVertexBuffer(const CommandBuffer& buffer)
{
    VkBuffer     vertexBuffers[] = {m_Buffer};
    VkDeviceSize offsets[]       = {0};
    vkCmdBindVertexBuffers(buffer.GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
}

void DataBuffer::InitAsIndexBuffer(const CommandBuffer& buffer)
{
    vkCmdBindIndexBuffer(buffer.GetCommandBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
}
