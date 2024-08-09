//
// Created by theom on 08.04.2024.
//
#include "Scene.h"
#include "vulkanbase/VulkanBase.h"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_FORCE_RADIANS

void Scene::InitMeshes(Camera &cam, const CommandPool &pool, VkQueue graphicsQueue)
{
    //m_Meshes.emplace_back(std::move(std::make_unique<Mesh>(glm::vec2{1, 1}, 0.5f, 32)));
    //m_Meshes.emplace_back(std::move(std::make_unique<Mesh>(glm::vec2{-0.5f, -0.5f}, 1.f, 1.f)));
    m_Ubo.model = glm::mat4(1.f);
    m_Ubo.proj  = glm::mat4(1.f);
    m_3DUbo.proj = cam.m_ProjectionMatrix;
    const float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) /
                              static_cast<float>(VulkanBase::swapChainExtent.height);
    glm::vec3   scale(1 / aspectRatio, 1.f, 1.f);
    m_Ubo.view = glm::scale(glm::mat4(1.f), scale);
    m_Meshes3D.push_back(std::move(std::make_unique<Mesh3D>()));
    m_Meshes3D.back()->InitPlane({0,0,0},1.f);
    m_Meshes3D.back()->LoadTexture(pool, graphicsQueue, "Models/vehicle_diffuse.png", "Models/white.png",
                                   "Models/white.png", "Models/white.png");
    m_Meshes3D.back()->InitDescriptor();
    m_Meshes3D.push_back(std::move(std::make_unique<Mesh3D>()));
    m_Meshes3D.back()->LoadModel("Models/vehicle.obj");
    m_Meshes3D.back()->LoadTexture(pool, graphicsQueue, "Models/vehicle_diffuse.png", "Models/vehicle_normal.png",
                                   "Models/vehicle_gloss.png", "Models/vehicle_specular.png");
    m_Meshes3D.back()->InitDescriptor();
    m_Meshes3D.back()->Translate({-6,-6,-6});
    m_pAlbedoMap = std::make_unique<dae::Texture>("Models/vehicle_diffuse.png", pool, graphicsQueue);
    m_pNormalMap = std::make_unique<dae::Texture>("Models/white.png", pool, graphicsQueue);
    m_pRoughnessMap = std::make_unique<dae::Texture>("Models/white.png", pool, graphicsQueue);
    m_pSpecularMap = std::make_unique<dae::Texture>("Models/white.png", pool, graphicsQueue);

    m_DescriptorPool.InitSampler(m_pAlbedoMap->GetSampler(), m_pNormalMap->GetSampler(),m_pRoughnessMap->GetSampler(),m_pSpecularMap->GetSampler());
    m_DescriptorPool.InitImageView(m_pAlbedoMap->GetImageView(), m_pNormalMap->GetImageView(),m_pRoughnessMap->GetImageView(),m_pSpecularMap->GetImageView());

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
    m_pAlbedoMap->CleanUp();
    m_pNormalMap->CleanUp();
    m_pSpecularMap->CleanUp();
    m_pRoughnessMap->CleanUp();
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

void Scene::Update(Camera &cam, uint32_t currentFrame)
{
    m_3DUbo.view = cam.m_ViewMatrix;
    for(const auto& mesh3d : m_Meshes3D) {
        mesh3d->Update(m_3DUbo,currentFrame);
    }


}
