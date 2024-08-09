//
// Created by theom on 05.04.2024.
//

#ifndef VULKANLAB01_COMMANDBUFFER_H
#define VULKANLAB01_COMMANDBUFFER_H


#include "CommandPool.h"
#include <vulkanbase/VulkanUtil.h>

class VulkanBase;

class CommandBuffer
{
public:
    CommandBuffer(const CommandPool &commandPool, VulkanBase *vulkanBase);
    CommandBuffer(const CommandBuffer &) = delete;
    CommandBuffer(CommandBuffer &&) = delete;
    CommandBuffer &operator=(const CommandBuffer &) = delete;
    CommandBuffer &operator=(CommandBuffer &&) = delete;
    ~CommandBuffer() = default;
    [[nodiscard]] VkCommandBuffer GetCommandBuffer() const;
    [[nodiscard]] const VkCommandBuffer *const GetCommandBufferRef() const;
    void RecordCommandBuffer(uint32_t imageIndex, VulkanBase *vulkanBase);
    void SetCommandBuffer(const VkCommandBuffer &buffer);
    void BeginRecording() const;
    void EndRecording() const;
    void Reset() const;
    void Submit(VkSubmitInfo &info) const;
    static VkCommandBuffer BeginSingleTimeCommands(const CommandPool& pool);
    static void EndSingleTimeCommands(VkCommandBuffer buffer, VkQueue graphicsQueue, const CommandPool& pool);
private:

    void CreateBuffer(const CommandPool &commandPool);
    VkCommandBuffer m_CommandBuffer;
};


#endif //VULKANLAB01_COMMANDBUFFER_H
