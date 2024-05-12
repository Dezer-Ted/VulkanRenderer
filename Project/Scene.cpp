//
// Created by theom on 08.04.2024.
//
#include "Scene.h"
#include "vulkanbase/VulkanBase.h"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_FORCE_RADIANS

void Scene::InitMeshes()
{
    m_Meshes.emplace_back(std::move(std::make_unique<Mesh>(glm::vec2{1, 1}, 0.5f, 32)));
    m_Meshes.emplace_back(std::move(std::make_unique<Mesh>(glm::vec2{-0.5f, -0.5f}, 1.f, 1.f)));
    m_Ubo.model = glm::mat4(1.f);
    m_Ubo.proj  = glm::mat4(1.f);
    const float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) /
                              static_cast<float>(VulkanBase::swapChainExtent.height);
    glm::vec3   scale(1 / aspectRatio, 1.f, 1.f);
    m_Ubo.view = glm::scale(glm::mat4(1.f), scale);
    m_Meshes3D.push_back(std::move(std::make_unique<Mesh3D>()));
    m_Meshes3D.back()->InitCube({-0.5f, -0.5f, -0.5f}, 1.f);
    m_Meshes3D.push_back(std::move(std::make_unique<Mesh3D>()));
    m_Meshes3D.back()->LoadModel("Models/vehicle.obj");
    m_Meshes3D.back()->Translate({-6,-6,-6});
    m_DescriptorPool.Initialize();

}

void Scene::Draw(const CommandBuffer& buffer, uint32_t currentFrame, const GraphicsPipeline& pipeline2D,
                 const GraphicsPipeline& pipeline3D)
{
    vkCmdBindPipeline(buffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline2D.GetGraphicsPipeline());
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, m_Ubo);
    for (auto& mesh: m_Meshes)
    {
        mesh->Draw(buffer, currentFrame, m_DescriptorPool, pipeline2D);
    }
    vkCmdBindPipeline(buffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline3D.GetGraphicsPipeline());
    for (auto& mesh: m_Meshes3D)
    {
        mesh->Draw(buffer, currentFrame, pipeline3D, m_3DUbo);
    }
}

void Scene::Destroy()
{
    for (auto& mesh: m_Meshes)
    {
        mesh->Destroy();
    }
    for (auto& mesh: m_Meshes3D)
    {
        mesh->Destroy();
    }
    m_DescriptorPool.Destroy();
}

void Scene::CompoundUpload(const CommandPool& pool, const VkQueue& graphicsQueue)
{
    for (auto& mesh: m_Meshes)
    {
        mesh->UploadBuffer(pool, graphicsQueue);
    }
    for (auto& mesh: m_Meshes3D)
    {
        mesh->UploadBuffer(pool, graphicsQueue);
    }
}

void Scene::Update(Camera& cam)
{
    cam.Update();
    m_Meshes3D.back()->Translate(cam.position);
    //m_3DUbo.view = glm::lookAt(cam.position, {0, 0, 0}, {0, 1, 0});

//    m_3DUbo.view = cam.GetViewMatrix();
//    m_3DUbo.proj = glm::perspective(glm::radians(70.f), static_cast<float>(VulkanBase::swapChainExtent.width) /
//                                                        static_cast<float>(VulkanBase::swapChainExtent.height), 1000.f,
//                                    0.1f);
//    m_3DUbo.proj[1][1] *= -1;
}
