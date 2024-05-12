//
// Created by theom on 08.04.2024.
//

#ifndef VULKANLAB01_SCENE_H
#define VULKANLAB01_SCENE_H


#include <vector>
#include "Mesh.h"
#include "DescriptorPool.h"
#include "Mesh3D.h"
#include "Camera.h"

class Scene
{
public:
    Scene() = default;
    Scene(const Scene& ) = delete;
    Scene(Scene&& ) = delete;
    Scene& operator=(const Scene& ) = delete;
    Scene& operator=(Scene&& ) = delete;
    ~Scene() = default;
    void InitMeshes();
    void Draw(const CommandBuffer& buffer, uint32_t currentFrame, const GraphicsPipeline& pipeline2D,
              const GraphicsPipeline& pipeline3D);
    void Destroy();
    void CompoundUpload(const CommandPool& pool, const VkQueue& graphicsQueue);
    void Update(Camera& cam);

private:
    DescriptorPool m_DescriptorPool{};
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::vector<std::unique_ptr<Mesh3D>> m_Meshes3D;
    VertexUBO m_Ubo{};
    VertexUBO m_3DUbo{};
};


#endif //VULKANLAB01_SCENE_H
