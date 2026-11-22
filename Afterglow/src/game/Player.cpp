#include "game/Player.h"
#include "game/Level.h"

#include "core/Config.h"
#include "platform/Input.h"

namespace ag
{
    void Player::Reset(const glm::vec3& spawnPosition)
    {
        m_camera = Camera{};
        m_camera.SetPosition(spawnPosition);
    }

    glm::vec3 Player::FeetPosition() const
    {
        return m_camera.Position() - glm::vec3(0.0f, config::PlayerEyeHeight, 0.0f);
    }

    void Player::Update(float deltaSeconds, const Input& input, const Level& level)
    {
        m_camera.AddLook(input.MouseDeltaX() * config::MouseSensitivity,
                         input.MouseDeltaY() * config::MouseSensitivity);

        glm::vec3 forward = m_camera.Forward();
        forward.y = 0.0f;
        forward = glm::normalize(forward);
        const glm::vec3 right = m_camera.Right();

        glm::vec3 direction{ 0.0f };
        if (input.IsKeyDown(GLFW_KEY_W))
            direction += forward;
        if (input.IsKeyDown(GLFW_KEY_S))
            direction -= forward;
        if (input.IsKeyDown(GLFW_KEY_D))
            direction += right;
        if (input.IsKeyDown(GLFW_KEY_A))
            direction -= right;

        if (glm::dot(direction, direction) == 0.0f || deltaSeconds == 0.0f)
            return;

        const glm::vec3 displacement = glm::normalize(direction)
            * (config::PlayerWalkSpeed * deltaSeconds);

        constexpr float maxStepDistance = 0.1f;
        const int stepCount = std::max(1, static_cast<int>(std::ceil(glm::length(displacement) / maxStepDistance)));
        const glm::vec3 step = displacement / static_cast<float>(stepCount);

        const glm::vec3 bodyHalfSize(0.25f, 0.9f, 0.25f);
        const glm::vec3 eyeToBodyCenter(0.0f, bodyHalfSize.y - config::PlayerEyeHeight, 0.0f);

        glm::vec3 position = m_camera.Position();
        for (int i = 0; i < stepCount; ++i)
        {
            glm::vec3 candidate = position;
            candidate.x += step.x;
            if (!level.IntersectsWall(candidate + eyeToBodyCenter, bodyHalfSize))
                position = candidate;

            candidate = position;   // X 이동 결과를 반영한 위치에서 Z를 검사한다.
            candidate.z += step.z;
            if (!level.IntersectsWall(candidate + eyeToBodyCenter, bodyHalfSize))
                position = candidate;
        }

        m_camera.SetPosition(position);
    }
}
