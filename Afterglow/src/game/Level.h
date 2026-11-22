#pragma once

#include "core/Config.h"

namespace ag
{
    struct Wall
    {
        glm::vec3 center;
        glm::vec3 size;
        glm::vec3 color{ 1.0f };
    };

    struct RoomDesc
    {
        glm::vec2 center;   // XZ 평면에서의 중심
        glm::vec2 size;     // 벽 안쪽 기준 X축 너비, Z축 길이
        float height;
        glm::vec3 wallColor{ 0.55f, 0.62f, 0.65f };
    };

    // North: +Z, South: -Z, East: +X, West: -X
    enum class WallSide { North, South, East, West };

    struct DoorwayDesc
    {
        WallSide side;
        float offset;   // N/S벽에서는 +X, E/W벽에서는 +Z 방향이 양수
        float width;
        float height;
    };

    // 실험실 방 4~5개와 연결 복도를 배치하고, 각 오브젝트의 위치를 제공한다.
    // 레벨 전용 수치(방 크기, 복도 길이)는 여기에만 둔다.
    class Level
    {
    public:
        void Initialize();
        bool IntersectsWall(const glm::vec3& center, const glm::vec3& halfSize) const;
        const std::vector<Wall>& Walls() const { return m_walls; }
        const std::vector<Wall>& Surfaces() const { return m_surfaces; }
        const glm::vec3& SpawnPosition() const { return m_spawnPosition; }

    private:
        std::vector<Wall> m_walls;
        // 바닥/천장은 수평 이동의 벽 충돌 대상과 분리한다.
        std::vector<Wall> m_surfaces;
        glm::vec3 m_spawnPosition{ 0.0f, config::PlayerEyeHeight, -2.0f };

        void AddRoom(const RoomDesc& room, const std::vector<DoorwayDesc>& doors = {});
    };
}
