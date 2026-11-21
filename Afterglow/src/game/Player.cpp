#include "game/Player.h"

#include "core/Config.h"
#include "platform/Input.h"

namespace ag
{
    void Player::Reset(const glm::vec3& spawnPosition)
    {
        m_camera = Camera{};
        m_camera.SetPosition(spawnPosition);
    }

    void Player::Update(float deltaSeconds, const Input& input)
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

        if (glm::dot(direction, direction) > 0.0f)
        {
            const glm::vec3 displacement = glm::normalize(direction)
                * (config::PlayerWalkSpeed * deltaSeconds);
            m_camera.SetPosition(m_camera.Position() + displacement);
        }
    }
}
