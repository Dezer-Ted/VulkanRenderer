//
// Created by theom on 20.02.2024.
//

#include "ShaderModule.h"
#include "vulkanbase/VulkanBase.h"
#include "vulkanbase/VulkanUtil.h"

ShaderModule::ShaderModule(const std::string& vertShaderPath, const std::string& fragShaderPath, const VkDevice& device)
        : m_Device{device}
{
    CreateVertexShaderInfo(vertShaderPath);
    CreateFragmentShaderInfo(fragShaderPath);

}

VkPipelineShaderStageCreateInfo ShaderModule::GetMFragShaderModule() const
{
    return m_FragmentShaderStageCreateInfo;
}

VkPipelineShaderStageCreateInfo ShaderModule::GetMVertexShaderModule() const
{
    return m_VertexShaderStageCreateInfo;
}

VkPipelineVertexInputStateCreateInfo ShaderModule::CreateVertexInputStateInfo2D()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    static auto bindingDescription    = Vertex2D::GetBindingDescription();
    static auto attributeDescriptions = Vertex2D::GetAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo ShaderModule::CreateInputAssemblyStateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

void ShaderModule::CreateVertexShaderInfo(const std::string& vertexShaderPath)
{
    std::vector<char> vertShaderCode = readFile(vertexShaderPath);
    m_VertShaderModule = CreateShaderModule(vertShaderCode);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = m_VertShaderModule;
    vertShaderStageInfo.pName  = "main";
    m_VertexShaderStageCreateInfo = vertShaderStageInfo;
}

void ShaderModule::CreateFragmentShaderInfo(const std::string& fragmentShaderPath)
{
    std::vector<char> fragShaderCode = readFile(fragmentShaderPath);
    m_FragShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = m_FragShaderModule;
    fragShaderStageInfo.pName  = "main";
    m_FragmentShaderStageCreateInfo = fragShaderStageInfo;
}

VkShaderModule ShaderModule::CreateShaderModule(const std::vector<char>& code)
{

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void ShaderModule::CreateDiscriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding albedoSamplerLayoutBinding{};
    albedoSamplerLayoutBinding.binding = 1;
    albedoSamplerLayoutBinding.descriptorCount = 1;
    albedoSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedoSamplerLayoutBinding.pImmutableSamplers = nullptr;
    albedoSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding normalSamplerLayoutBinding{};
    normalSamplerLayoutBinding.binding = 2;
    normalSamplerLayoutBinding.descriptorCount = 1;
    normalSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalSamplerLayoutBinding.pImmutableSamplers = nullptr;
    normalSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding roughnessSamplerLayoutBinding{};
    roughnessSamplerLayoutBinding.binding = 3;
    roughnessSamplerLayoutBinding.descriptorCount = 1;
    roughnessSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    roughnessSamplerLayoutBinding.pImmutableSamplers = nullptr;
    roughnessSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding specularSamplerLayoutBinding{};
    specularSamplerLayoutBinding.binding = 4;
    specularSamplerLayoutBinding.descriptorCount = 1;
    specularSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    specularSamplerLayoutBinding.pImmutableSamplers = nullptr;
    specularSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding,5> bindings = {
                                                           uboLayoutBinding,
                                                           albedoSamplerLayoutBinding,
                                                           normalSamplerLayoutBinding,
                                                           roughnessSamplerLayoutBinding,
                                                           specularSamplerLayoutBinding
                                                            };


    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();
    if (vkCreateDescriptorSetLayout(VulkanBase::device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout");
    }
}

VkDescriptorSetLayout ShaderModule::GetDescriptorSetLayout() const
{
    return m_DescriptorSetLayout;
}

void ShaderModule::Destroy()
{
    vkDestroyShaderModule(VulkanBase::device, m_VertShaderModule, nullptr);
    vkDestroyShaderModule(VulkanBase::device, m_FragShaderModule, nullptr);
}

VkPipelineVertexInputStateCreateInfo ShaderModule::CreateVertexInputStateInfo3D()
{

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    static auto bindingDescription    = Vertex3D::GetBindingDescription();
    static auto attributeDescriptions = Vertex3D::GetAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
    return vertexInputInfo;
}

void ShaderModule::DestroyDescriptorLayout()
{
    vkDestroyDescriptorSetLayout(VulkanBase::device, m_DescriptorSetLayout, nullptr);
}
