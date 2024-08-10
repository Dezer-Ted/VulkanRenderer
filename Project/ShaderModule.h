//
// Created by theom on 20.02.2024.
//

#ifndef VULKANLAB01_SHADERMODULE_H
#define VULKANLAB01_SHADERMODULE_H

#include <vulkanbase/VulkanUtil.h>
#include <string>
#include<vector>
#include "DataBuffer.h"
enum class ShaderModuleType {
    Shader2D,
    Shader3D
};
class ShaderModule
{
public:
    ShaderModule(const std::string& vertShaderPath, const std::string& fragShaderPath, const VkDevice& device);
    ShaderModule(const ShaderModule& shaderModule) = delete;
    ShaderModule(ShaderModule&& shaderModule) = delete;
    ShaderModule operator=(const ShaderModule& shaderModule) = delete;
    ShaderModule operator=(ShaderModule&& shaderModule) = delete;
    ~ShaderModule() = default;

    [[nodiscard]] VkPipelineShaderStageCreateInfo GetMFragShaderModule() const;
    [[nodiscard]] VkPipelineShaderStageCreateInfo GetMVertexShaderModule() const;
    VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
    VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo2D();
    VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo3D();
    [[nodiscard]]VkDescriptorSetLayout GetDescriptorSetLayout() const;
    void CreateDiscriptorSetLayout();
    void Destroy();
    void DestroyDescriptorLayout();
    ShaderModuleType shaderType{};
    inline static VkDescriptorSetLayout m_DescriptorSetLayout;
private:
    VkPipelineShaderStageCreateInfo m_FragmentShaderStageCreateInfo;
    VkPipelineShaderStageCreateInfo m_VertexShaderStageCreateInfo;
    VkShaderModule                  m_VertShaderModule;
    VkShaderModule                  m_FragShaderModule;
    VkDevice                        m_Device;
    void CreateFragmentShaderInfo(const std::string& FragmentShaderPath);
    void CreateVertexShaderInfo(const std::string& vertexShaderPath);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    DataBuffer                      m_UBOBuffer{
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

};

#endif //VULKANLAB01_SHADERMODULE_H
