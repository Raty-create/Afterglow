#pragma once

#include "core/Config.h"

#include <array>
#include <vector>

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
        float floorY = 0.0f;
    };

    // North: +Z, South: -Z, East: +X, West: -X
    enum class WallSide { North, South, East, West };

    struct DoorwayDesc
    {
        WallSide side;
        float offset;   // N/S벽에서는 +X, E/W벽에서는 +Z 방향이 양수
        float width;
        float height;
        float bottom = 0.0f; // 방 바닥으로부터 출입구까지의 높이
    };

    // 연구동과 지원 공간을 배치하고, 각 오브젝트의 위치를 제공한다.
    // 레벨 전용 수치(방 크기, 복도 길이)는 여기에만 둔다.
    class Level
    {
    public:
        void Initialize();
        bool IntersectsWall(const glm::vec3& center, const glm::vec3& halfSize) const;
        bool BlocksInteraction(const glm::vec3& from, const glm::vec3& to) const;
        bool TryGetFloor(glm::vec2 position, float currentY, float& floorY) const;
        bool ReachedExit(const glm::vec3& feet) const;
        const std::vector<Wall>& Walls() const { return m_walls; }
        const std::vector<Wall>& Surfaces() const { return m_surfaces; }
        const std::vector<Wall>& Fixtures() const { return m_fixtures; }
        const glm::vec3& SpawnPosition() const { return m_spawnPosition; }
        const auto& BatteryPositions() const { return m_batteryPositions; }
        const auto& LeverPositions() const { return m_leverPositions; }

    private:
        struct WalkSurface
        {
            glm::vec2 min;
            glm::vec2 max;
            float southY;
            float northY;
        };

        std::vector<Wall> m_walls;
        // 바닥, 천장, 안내 표식은 수평 이동의 충돌 대상과 분리한다.
        std::vector<Wall> m_surfaces;
        std::vector<Wall> m_fixtures;
        std::vector<Wall> m_collisionOnly;
        std::vector<WalkSurface> m_walkSurfaces;
        glm::vec3 m_spawnPosition{ 0.0f, config::PlayerEyeHeight, -2.0f };
        std::array<glm::vec3, config::BatteryPickupCount> m_batteryPositions{};
        std::array<glm::vec3, config::LeverCount> m_leverPositions{};

        void AddRoom(const RoomDesc& room, const std::vector<DoorwayDesc>& doors = {});
        void AddLobbyWing();
        void AddFloor(glm::vec2 min, glm::vec2 max, float y, const glm::vec3& color);
    };
}
