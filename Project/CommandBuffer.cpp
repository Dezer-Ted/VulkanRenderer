//
// Created by theom on 05.04.2024.
//

#include "CommandBuffer.h"
#include "vulkanbase/VulkanBase.h"

CommandBuffer::CommandBuffer(const CommandPool &commandPool, VulkanBase *vulkanBase)
{
    CreateBuffer(commandPool);
}

void CommandBuffer::RecordCommandBuffer(uint32_t imageIndex, VulkanBase *vulkanBase)
{
    vulkanBase->DrawFrame(imageIndex);
}

void CommandBuffer::CreateBuffer(const CommandPool &commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool.GetCommandPool();
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VulkanBase::device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

VkCommandBuffer CommandBuffer::GetCommandBuffer() const
{
    return m_CommandBuffer;
}

const VkCommandBuffer *const CommandBuffer::GetCommandBufferRef() const
{
    return &m_CommandBuffer;
}

void CommandBuffer::SetCommandBuffer(VkCommandBuffer const &buffer)
{
    m_CommandBuffer = buffer;
}

void CommandBuffer::BeginRecording() const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandBuffer::EndRecording() const
{
    if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffer::Reset() const
{
    vkResetCommandBuffer(m_CommandBuffer, 0);
}

void CommandBuffer::Submit(VkSubmitInfo &info) const
{
    info.commandBufferCount = 1;
    info.pCommandBuffers    = &m_CommandBuffer;
}
