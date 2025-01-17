//
// Created by theom on 06.08.2024.
//

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "vulkanbase/VulkanBase.h"
void dae::Texture::CreateTextureImage(const std::string& path ) {
    int texWidth;
    int texHeight;
    int texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(),&texWidth,&texHeight,&texChannels,STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth* texHeight*4;
    if(!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }
    DataBuffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,m_StagingBuffer,m_StagingBufferMemory);
    void* data;
    vkMapMemory(VulkanBase::device, m_StagingBufferMemory,0, imageSize,0,&data);
    memcpy(data,pixels,static_cast<size_t>(imageSize));
    vkUnmapMemory(VulkanBase::device,m_StagingBufferMemory);
    stbi_image_free(pixels);

    CreateImage(texWidth,texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,m_TextureImage,m_TextureImageMemory
                );
    TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          m_TextureImage, m_Pool, m_GraphicsQueue);
    CopyBufferToImage(m_StagingBuffer,m_TextureImage,static_cast<uint32_t>(texWidth),static_cast<uint32_t>(texHeight));
    TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TextureImage, m_Pool, m_GraphicsQueue);

    vkDestroyBuffer(VulkanBase::device, m_StagingBuffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_StagingBufferMemory, nullptr);

}


dae::Texture::Texture(const std::string & path, const CommandPool& pool,VkQueue graphicsQueue) :
    m_Pool{ pool},
    m_GraphicsQueue{graphicsQueue}
{
    CreateTextureImage(path);
    m_TextureImageView = CreateTextureImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    CreateTextureSampler();
}

void
dae::Texture::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags props, VkImage &image, VkDeviceMemory &imageMem) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateImage(VulkanBase::device, &imageInfo, nullptr, &image)!= VK_SUCCESS) {
        throw std::runtime_error("failed to create Image!");
    }

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(VulkanBase::device, image,&memReq);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = DataBuffer::FindMemoryType(memReq.memoryTypeBits,props);
    if(vkAllocateMemory(VulkanBase::device,&allocInfo,nullptr,&imageMem) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image Memory");
    }
    vkBindImageMemory(VulkanBase::device,image,imageMem,0);


}

void
dae::Texture::TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image,
                                    const CommandPool &pool, VkQueue graphicsQueue) {
    VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands(pool);


    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    //barrier.srcAccessMask = 0;
    //barrier.dstAccessMask = 0;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if(DepthBuffer::HasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if(oldLayout== VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer,
                         sourceStage,destinationStage,
                         0,
                         0,nullptr,
                         0,nullptr,
                         1,&barrier
                         );

    CommandBuffer::EndSingleTimeCommands(commandBuffer,graphicsQueue,pool);
}

void dae::Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands(m_Pool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0,0,0};
    region.imageExtent = {width, height, 1};



    vkCmdCopyBufferToImage(commandBuffer,
                           buffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region
                           );

    CommandBuffer::EndSingleTimeCommands(commandBuffer,m_GraphicsQueue,m_Pool);

}

void dae::Texture::CleanUp() {
    vkDestroyImage(VulkanBase::device,m_TextureImage, nullptr);
    vkFreeMemory(VulkanBase::device, m_TextureImageMemory,nullptr);
    vkDestroySampler(VulkanBase::device, m_TextureSampler, nullptr);
    vkDestroyImageView(VulkanBase::device, m_TextureImageView, nullptr);
}

VkImageView dae::Texture::CreateTextureImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    VkImageView imageView;
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    //viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.format = format;
    //viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if(vkCreateImageView(VulkanBase::device, &viewInfo,nullptr,&imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    return imageView;
}

void dae::Texture::CreateTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(VulkanBase::physicalDevice,&props);


    samplerInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = 0.f;

    if(vkCreateSampler(VulkanBase::device, &samplerInfo, nullptr,&m_TextureSampler)) {
        throw std::runtime_error("failed to create texture sampler");
    }



}

VkImageView dae::Texture::GetImageView() const {
    return m_TextureImageView;
}

VkSampler dae::Texture::GetSampler() const {
    return m_TextureSampler;
}
