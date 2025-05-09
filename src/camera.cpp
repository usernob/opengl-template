#include "camera.h"
#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

void Camera::processKeyboard(CameraMovement direction, float delta_time)
{
    float velocity = m_movement_speed * delta_time;
    switch (direction)
    {
        case CameraMovement::FORWARD:
            m_position += m_front * velocity;
            break;
        case CameraMovement::BACKWARD:
            m_position -= m_front * velocity;
            break;
        case CameraMovement::LEFT:
            m_position -= m_right * velocity;
            break;
        case CameraMovement::RIGHT:
            m_position += m_right * velocity;
            break;
        case CameraMovement::UP:
            m_position += m_up * velocity;
            break;
        case CameraMovement::DOWN:
            m_position -= m_up * velocity;
            break;
        default:
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrain_pitch)
{
    xoffset *= m_mouse_sesivity;
    yoffset *= m_mouse_sesivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrain_pitch)
    {
        m_pitch = glm::min(glm::max(m_pitch, -89.f), 89.f);
    }
    mUpdateCameraVector();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;
    m_zoom = glm::min(glm::max(m_zoom, 1.f), 45.f);
}

void Camera::mUpdateCameraVector()
{
    glm::vec3 front;
    front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    front.y = glm::sin(glm::radians(m_pitch));
    front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldup));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
