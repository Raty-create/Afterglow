#pragma once

#include "render/Camera.h"

namespace ag
{
    class Input;

    // 플레이어 이동과 시점 회전을 담당한다.
    class Player
    {
    public:
        void Reset(const glm::vec3& spawnPosition);
        void Update(float deltaSeconds, const Input& input);
        const Camera& ViewCamera() const { return m_camera; }

    private:
        Camera m_camera;
    };
}
