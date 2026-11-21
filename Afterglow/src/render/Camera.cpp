#include "render/Camera.h"

#include "core/Config.h"

#include <cmath>

namespace ag
{
    void Camera::AddLook(float deltaXDeg, float deltaYDeg)
    {
        // +Z를 볼 때 뷰 행렬의 오른쪽은 -X 방향이다.
        m_yawDeg = std::fmod(m_yawDeg - deltaXDeg, 360.0f);
        m_pitchDeg -= deltaYDeg;

        // 위아래를 완전히 뒤집지 않도록 제한한다.
        if (m_pitchDeg > config::PitchLimitDeg)
            m_pitchDeg = config::PitchLimitDeg;
        if (m_pitchDeg < -config::PitchLimitDeg)
            m_pitchDeg = -config::PitchLimitDeg;
    }

    glm::vec3 Camera::Forward() const
    {
        const float yaw = glm::radians(m_yawDeg);
        const float pitch = glm::radians(m_pitchDeg);
        return { std::sin(yaw) * std::cos(pitch), std::sin(pitch), std::cos(yaw) * std::cos(pitch) };
    }

    glm::vec3 Camera::Right() const
    {
        const float yaw = glm::radians(m_yawDeg);
        return { -std::cos(yaw), 0.0f, std::sin(yaw) };
    }

    glm::mat4 Camera::ViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + Forward(), { 0.0f, 1.0f, 0.0f });
    }

    glm::mat4 Camera::ProjectionMatrix(float aspect) const
    {
        return glm::perspective(glm::radians(m_fovYDeg), aspect, m_nearZ, m_farZ);
    }
}
