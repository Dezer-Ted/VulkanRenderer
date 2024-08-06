//
// Created by theom on 09.04.2024.
//

#ifndef VULKANLAB01_CAMERA_H
#define VULKANLAB01_CAMERA_H

#include "vulkanbase/VulkanUtil.h"

class Camera
{
public:
    Camera(float fov ,glm::vec3 position, float aspectRatio);
     glm::vec3 m_Forward {0.f,0.f,1.f};
     glm::vec3 m_Up{0.f,1.f,0.f};
     glm::vec3 m_Right {1.f,0.f,0.f};
    const float m_NearPlane{0.1f};
    const float m_FarPlane{10000};
    const float m_FOVRadian;
    const float m_Speed{1.f};
    const float m_RotationSpeed{0.1f};
    const float m_AspectRatio{};

    glm::vec3 m_Position{};
    float     m_Pitch{0.f};
    float     m_Yaw{0.f};
    glm::mat4 m_ViewMatrix{};
    glm::mat4 m_ProjectionMatrix{};
    void CalculateViewMatrix();
    void CalculateProjMatrix();
    void UpdateYaw(float moveDelta);
    void UpdatePitch(float moveDelta);
    void UpdateRotation();
    void Update();
};


#endif //VULKANLAB01_CAMERA_H
