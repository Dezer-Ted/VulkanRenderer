//
// Created by theom on 05.04.2024.
//

#ifndef VULKANLAB01_DATABUFFER_H
#define VULKANLAB01_DATABUFFER_H

#include "vulkanbase/VulkanUtil.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

class DataBuffer
{
public:
    DataBuffer(const VkBufferUsageFlags& usageFlags, const VkMemoryPropertyFlags& properties);
    DataBuffer(const DataBuffer&) = delete;
    DataBuffer(DataBuffer&&) = delete;
    DataBuffer& operator=(const DataBuffer&) = delete;
    DataBuffer& operator=(DataBuffer&&) = delete;
    ~DataBuffer() = default;
    [[nodiscard]] VkBuffer GetVertexBuffer() const;
    void CleanUp();
    void MapMemory(VkDeviceSize size, void* data);
    void Upload(const VkQueue& graphicsQueue, const CommandPool& pool);
    void Destroy();
    void InitAsVertexBuffer(const CommandBuffer& buffer);
    static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buffer,
                      VkDeviceMemory& buffermemory);
    void InitAsIndexBuffer(const CommandBuffer& buffer);
    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:

    void
    CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool, VkQueue graphicsQueue);
    VkBufferUsageFlags    m_UsageFlags;
    VkMemoryPropertyFlags m_Properties;
    VkBuffer              m_Buffer;
    VkDeviceMemory        m_Memory;
    VkBuffer              m_StagingBuffer;
    VkDeviceMemory        m_StagingBufferMemory;
    VkDeviceSize          m_Size{};
};


#endif //VULKANLAB01_DATABUFFER_H
