#pragma once



namespace ag
{
    // 1인칭 카메라. 위치와 yaw/pitch로 시점을 표현하고 뷰/투영 행렬을 만든다.
    class Camera
    {
    public:
        void SetPosition(const glm::vec3& position) { m_position = position; }
        const glm::vec3& Position() const { return m_position; }

        // 마우스 이동량(픽셀)을 받아 회전한다. pitch는 상하 한계로 제한된다.
        void AddLook(float deltaXDeg, float deltaYDeg);

        void SetFovY(float degrees) { m_fovYDeg = degrees; }
        void SetClipPlanes(float nearZ, float farZ) { m_nearZ = nearZ; m_farZ = farZ; }

        glm::vec3 Forward() const;
        glm::vec3 Right() const;

        glm::mat4 ViewMatrix() const;
        glm::mat4 ProjectionMatrix(float aspect) const;

    private:
        glm::vec3  m_position{};
        float m_yawDeg   = 0.0f;   // 좌우 회전 (y축)
        float m_pitchDeg = 0.0f;   // 상하 회전
        float m_fovYDeg  = 70.0f;
        float m_nearZ    = 0.05f;
        float m_farZ     = 100.0f;
    };
}
