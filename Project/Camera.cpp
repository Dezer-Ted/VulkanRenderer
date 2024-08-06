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
#include <AccCtrl.h>


void Camera::CalculateViewMatrix()
{
    m_ViewMatrix = glm::lookAtLH(m_Position, m_Position+m_Forward,m_Up);
}




void Camera::Update()
{
    UpdateRotation();
    CalculateViewMatrix();
}

Camera::Camera(float fov, glm::vec3 position, float aspectRatio) :
    m_FOVRadian{glm::radians(fov)},
    m_Position{position},
    m_AspectRatio{aspectRatio}
{
    CalculateProjMatrix();
}

void Camera::CalculateProjMatrix() {

    m_ProjectionMatrix = glm::mat4 {glm::perspectiveLH(m_FOVRadian,m_AspectRatio,m_NearPlane,m_FarPlane)};
    m_ProjectionMatrix[1][1] *= -1;
}

void Camera::UpdateYaw(float moveDelta) {
    if( abs(moveDelta)< 0.1 )
        return;
    m_Yaw += m_RotationSpeed * moveDelta;
    if( m_Yaw >= 360.f) {
        m_Yaw = 0.f;
    }
    else if ( m_Yaw <0.f) {
        m_Yaw = 360.f;
    }

}

void Camera::UpdatePitch(float moveDelta) {
    if( abs(moveDelta)< 0.1 )
        return;
    m_Pitch += m_RotationSpeed * moveDelta;
    if( m_Pitch >= 360.f) {
        m_Pitch = 0.f;
    }
    else if ( m_Pitch <0.f) {
        m_Pitch = 360.f;
    }
}

void Camera::UpdateRotation() {
    glm::mat4 rotationMatrix {};
    const glm::vec3 worldUp {0.f,1.f,0.f};
    rotationMatrix = glm::rotate(glm::mat4{1.f},glm::radians(m_Pitch),m_Right);
    rotationMatrix += glm::rotate( glm::mat4{1.f}, glm::radians(m_Yaw),worldUp);

    m_Forward = glm::vec3 {glm::normalize(rotationMatrix * glm::vec4{0.f,worldUp})};
    m_Right =glm::normalize(glm::cross(worldUp, m_Forward));
    m_Up = glm::normalize(glm::cross(m_Forward, m_Right));
}



