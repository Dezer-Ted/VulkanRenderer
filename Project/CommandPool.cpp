//
// Created by theom on 05.04.2024.
//

#include "CommandPool.h"
#include "vulkanbase/VulkanBase.h"


void CommandPool::CreateCommandPool(const QueueFamilyIndices &queueFamilyIndices)
{
//    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(VulkanBase::physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(VulkanBase::device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

CommandPool::CommandPool(const QueueFamilyIndices &queueFamilyIndices)
{
    CreateCommandPool(queueFamilyIndices);
}

VkCommandPool CommandPool::GetCommandPool() const
{
    return m_CommandPool;
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(VulkanBase::device, m_CommandPool, nullptr);
}

