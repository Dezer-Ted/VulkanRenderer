//
// Created by theom on 09.04.2024.
//

#ifndef VULKANLAB01_CAMERA_H
#define VULKANLAB01_CAMERA_H

#include "vulkanbase/VulkanUtil.h"

class Camera
{
public:
    glm::vec3 velocity;
    glm::vec3 position;
    float     pitch{0.f};
    float     yaw{0.f};
    glm::mat4 GetViewMatrix();
    glm::mat4 GetRotationMatrix();
    void Update();
};


#endif //VULKANLAB01_CAMERA_H
