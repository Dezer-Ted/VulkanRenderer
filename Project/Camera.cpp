//
// Created by theom on 09.04.2024.
//

#include "Camera.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/quaternion.hpp>
#include <DeltaTime.h>
#include <Singleton.h>


glm::mat4 Camera::GetViewMatrix()
{
    glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 cameraRotation    = GetRotationMatrix();
    return glm::inverse(cameraTranslation * cameraRotation);
}


glm::mat4 Camera::GetRotationMatrix()
{
    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{1.f, 0.f, 0.f});
    glm::quat yawRotation   = glm::angleAxis(yaw, glm::vec3{0.f, -1.f, 0.f});
    return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

void Camera::Update()
{
    glm::mat4 cameraRotation = GetRotationMatrix();
    position += glm::vec3(
            cameraRotation * glm::vec4(velocity * (dae::Singleton<DeltaTime>::GetInstance().GetDeltaTime()/1000), 0.f));
}
