//
// Created by theom on 08.04.2024.
//

#ifndef VULKANLAB01_GRAPHICSPIPELINE_H
#define VULKANLAB01_GRAPHICSPIPELINE_H

#include "vulkanbase/VulkanUtil.h"
#include "ShaderModule.h"

class GraphicsPipeline
{
public:
    GraphicsPipeline() = default;
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
    ~GraphicsPipeline() = default;
    void CreateRenderPass(const VkFormat& swapChainImageFormat);
    void CreateGraphicsPipeline(ShaderModule& shaderMod,
                                VkPipelineVertexInputStateCreateInfo vertInputStateCreateInfo);
    [[nodiscard]] VkPipeline GetGraphicsPipeline() const;
    [[nodiscard]]VkPipelineLayout GetLayout() const;
    VkRenderPass GetRenderPass();
    void DestroyPipeline();
    VkPipelineLayout m_PipelineLayout;
private:

    VkRenderPass m_RenderPass;
    VkPipeline   m_GraphicsPipeline;
};


#endif //VULKANLAB01_GRAPHICSPIPELINE_H
