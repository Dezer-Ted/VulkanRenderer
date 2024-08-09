//
// Created by theom on 07.08.2024.
//

#include "DepthBuffer.h"
#include "vulkanbase/VulkanBase.h"
void dae::DepthBuffer::CreateDepthResources() {
    VkFormat depthFormat = FindDepthFormat();
    Texture::CreateImage(m_SwapChainWidth,
                         m_SwapChainHeight,
                         depthFormat,VK_IMAGE_TILING_OPTIMAL,
                         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         m_DepthImage,
                         m_DepthImageMemory
                         );
    m_DepthImageView = Texture::CreateTextureImageView(m_DepthImage,depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT);
    Texture::TransitionImageLayout(depthFormat,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                   m_DepthImage,
                                   m_Pool,
                                   m_GraphicsQueue);
}

VkFormat dae::DepthBuffer::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                         VkFormatFeatureFlags features) {
    for (auto format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(VulkanBase::physicalDevice,format, &props);
        if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
        throw std::runtime_error("failed to find supported format!");
    }
}

VkFormat dae::DepthBuffer::FindDepthFormat() {
    return FindSupportedFormat({VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool dae::DepthBuffer::HasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

dae::DepthBuffer::DepthBuffer(uint32_t swapChainWidth, uint32_t swapChainHeight, const CommandPool &pool,
                              VkQueue graphicsQueue) :
    m_SwapChainHeight{swapChainHeight},
    m_SwapChainWidth{swapChainWidth},
    m_Pool{pool},
    m_GraphicsQueue{graphicsQueue}
{
    CreateDepthResources();
}

VkImageView dae::DepthBuffer::GetDepthView() const {
    return m_DepthImageView;
}

void dae::DepthBuffer::CleanUp() {
    vkDestroyImage(VulkanBase::device,m_DepthImage, nullptr);
    vkDestroyImageView(VulkanBase::device,m_DepthImageView,nullptr);
    vkFreeMemory(VulkanBase::device,m_DepthImageMemory,nullptr);
}
