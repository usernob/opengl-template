#ifndef SRC_CAMERA_H
#define SRC_CAMERA_H

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NONE
};
// Default camera values
static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 2.5f;
static const float SENSITIVITY = 0.1f;
static const float ZOOM = 45.0f;

class Camera
{
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldup;

    float m_yaw;
    float m_pitch;

    float m_movement_speed;
    float m_mouse_sesivity;

    float m_zoom;

public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    )
        : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movement_speed(SPEED),
          m_mouse_sesivity(SENSITIVITY), m_zoom(ZOOM)
    {
        m_position = position;
        m_worldup = up;
        m_yaw = yaw;
        m_pitch = pitch;
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::mat4 getProjectionMatrix(float aspect_ratio) const
    {
        return glm::perspective(glm::radians(m_zoom), aspect_ratio, 0.1f, 100.0f);
    }

    void processKeyboard(CameraMovement direction, float delta_time);
    void processMouseMovement(float xoffset, float yoffset, bool constrain_pitch = true);
    void processMouseScroll(float yoffset);

private:
    void mUpdateCameraVector();
};

#endif // SRC_CAMERA_H
