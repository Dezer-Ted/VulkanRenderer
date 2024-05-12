//
// Created by theom on 05.04.2024.
//

#ifndef VULKANLAB01_COMMANDPOOL_H
#define VULKANLAB01_COMMANDPOOL_H

#include <optional>
#include "vulkanbase/VulkanUtil.h"

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class CommandPool
{
public:
    explicit CommandPool(const QueueFamilyIndices &queueFamilyIndices);
    CommandPool(const CommandPool &) = delete;
    CommandPool(CommandPool &&) = delete;
    CommandPool &operator=(const CommandPool &) = delete;
    CommandPool &operator=(CommandPool &&) = delete;
    ~CommandPool();
    [[nodiscard]] VkCommandPool GetCommandPool() const;

private:
    void CreateCommandPool(const QueueFamilyIndices &queueFamilyIndices);
    VkCommandPool m_CommandPool;

};


#endif //VULKANLAB01_COMMANDPOOL_H
