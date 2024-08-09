//
// Created by theom on 06.08.2024.
//

#ifndef VULKANLAB01_TEXTURE_H
#define VULKANLAB01_TEXTURE_H
#include <string>
#include "DataBuffer.h"
namespace dae {
    class Texture {
    public:
        Texture(const std::string& path, const CommandPool& pool, VkQueue graphicsQueue);
        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        ~Texture() = default;
        void CleanUp();
        static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                                VkImageUsageFlags usage, VkMemoryPropertyFlags props,VkImage& image,VkDeviceMemory& imageMem);
        VkImageView GetImageView() const;
        VkSampler GetSampler() const;
        static VkImageView CreateTextureImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        static void
        TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image,
                              const CommandPool &pool, VkQueue graphicsQueue);
    private:
        void CreateTextureImage(const std::string& path );
        void CopyBufferToImage(VkBuffer buffer, VkImage image,uint32_t width, uint32_t height);
        void CreateTextureSampler();
        VkBuffer m_StagingBuffer;
        VkDeviceMemory m_StagingBufferMemory;
        VkImage m_TextureImage;
        VkDeviceMemory m_TextureImageMemory;
        const CommandPool& m_Pool;
        VkQueue m_GraphicsQueue;
        VkImageView m_TextureImageView;
        VkSampler m_TextureSampler;
    };

}


#endif //VULKANLAB01_TEXTURE_H
