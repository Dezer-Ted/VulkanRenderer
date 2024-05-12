//
// Created by theom on 09.04.2024.
//

#ifndef VULKANLAB01_VERTEX_H
#define VULKANLAB01_VERTEX_H

#include "vulkanbase/VulkanUtil.h"

struct Vertex2D
{
    alignas(16) glm::vec2 m_Pos{};
    alignas(16) glm::vec3 m_Color{};

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding   = 0;
        bindingDescription.stride    = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof(Vertex2D, m_Pos);

        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof(Vertex2D, m_Color);
        return attributeDescriptions;
    }
};

struct VertexUBO
{
    alignas(16)glm::mat4 model;
    alignas(16)glm::mat4 view;
    alignas(16)glm::mat4 proj;
};

struct Vertex3D
{
    alignas(16) glm::vec3 m_Pos{};
    alignas(16) glm::vec3 m_Color{};

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding   = 0;
        bindingDescription.stride    = sizeof(Vertex3D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof(Vertex3D, m_Pos);

        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 2;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof(Vertex3D, m_Color);

//        attributeDescriptions[2].binding  = 0;
//        attributeDescriptions[2].location = 4;
//        attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
//        attributeDescriptions[2].offset   = offsetof(Vertex3D, m_Color);
        return attributeDescriptions;
    }
};


#endif //VULKANLAB01_VERTEX_H
