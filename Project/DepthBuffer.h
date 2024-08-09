//
// Created by theom on 07.08.2024.
//

#ifndef VULKANLAB01_DEPTHBUFFER_H
#define VULKANLAB01_DEPTHBUFFER_H
#include "vulkanbase/VulkanUtil.h"
#include "CommandPool.h"
namespace dae {
    class DepthBuffer {
    public:
        DepthBuffer(uint32_t swapChainWidth, uint32_t swapChainHeight, const CommandPool &pool, VkQueue graphicsQueue);
        DepthBuffer(const DepthBuffer&) = delete;
        DepthBuffer(DepthBuffer&&) = delete;
        DepthBuffer& operator=(DepthBuffer&&) = delete;
        DepthBuffer& operator=(const DepthBuffer&) = delete;
        ~DepthBuffer() = default;
        void CleanUp();
        static bool HasStencilComponent(VkFormat format);
        VkImageView GetDepthView() const;
        static VkFormat FindDepthFormat();
    private:
        void CreateDepthResources();
        static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;
        VkQueue m_GraphicsQueue;
        uint32_t m_SwapChainWidth{};
        uint32_t m_SwapChainHeight{};
        const CommandPool& m_Pool;
    };

}


#endif //VULKANLAB01_DEPTHBUFFER_H
