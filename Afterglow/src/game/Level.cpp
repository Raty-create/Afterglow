#include "game/Level.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ag
{
    namespace
    {
        constexpr float WallThickness = 0.2f;
        const glm::vec3 FloorColor{ 0.24f, 0.28f, 0.30f };
        const glm::vec3 CeilingColor{ 0.68f, 0.70f, 0.69f };

        bool IsPositive(float value)
        {
            return std::isfinite(value) && value > 0.0f;
        }
    }

    void Level::Initialize()
    {
        m_walls.clear();
        m_surfaces.clear();

        // 배치 정보: 1단위=1m, 바닥 윗면 Y=0. 벽 좌표는 AddRoom에서 계산한다.
        const RoomDesc startRoom{ { 0.0f, 0.0f }, { 6.0f, 6.0f }, 3.0f };
        const RoomDesc corridor{ { 0.0f, 7.0f }, { 2.0f, 8.0f }, 3.0f, { 0.40f, 0.49f, 0.52f } };

        AddRoom(startRoom, { { WallSide::North, 0.0f, 2.0f, 2.3f } });
        // 복도 남쪽 전체를 열어 방과 연결한다. 문 위 벽은 시작 방이 생성한다.
        AddRoom(corridor, { { WallSide::South, 0.0f, 2.0f, 3.0f } });
    }

    void Level::AddRoom(const RoomDesc& room, const std::vector<DoorwayDesc>& doors)
    {
        if (!IsPositive(room.size.x) || !IsPositive(room.size.y) || !IsPositive(room.height)
            || !std::isfinite(room.center.x) || !std::isfinite(room.center.y))
            throw std::invalid_argument("Room dimensions must be positive and coordinates finite.");

        // 한 벽에 여러 출입구를 지원한다. 검증 후 벽별 위치 순으로 정렬한다.
        std::vector<DoorwayDesc> openings = doors;
        for (const DoorwayDesc& door : openings)
        {
            if (door.side != WallSide::North && door.side != WallSide::South
                && door.side != WallSide::East && door.side != WallSide::West)
                throw std::invalid_argument("Unknown wall side.");

            const bool alongX = door.side == WallSide::North || door.side == WallSide::South;
            const float length = alongX ? room.size.x : room.size.y;
            if (!IsPositive(door.width) || !IsPositive(door.height) || !std::isfinite(door.offset)
                || door.height > room.height || door.width > length
                || std::abs(door.offset) > (length - door.width) * 0.5f)
                throw std::invalid_argument("Doorway must fit inside its wall.");
        }
        std::sort(openings.begin(), openings.end(), [](const DoorwayDesc& a, const DoorwayDesc& b)
        {
            return a.side == b.side ? a.offset < b.offset : a.side < b.side;
        });
        for (size_t i = 1; i < openings.size(); ++i)
        {
            const auto& previous = openings[i - 1];
            const auto& current = openings[i];
            if (previous.side == current.side
                && previous.offset + previous.width * 0.5f > current.offset - current.width * 0.5f)
                throw std::invalid_argument("Doorways on the same wall must not overlap.");
        }

        const float halfThickness = WallThickness * 0.5f;
        m_surfaces.push_back({ { room.center.x, -halfThickness, room.center.y },
            { room.size.x, WallThickness, room.size.y }, FloorColor });
        m_surfaces.push_back({ { room.center.x, room.height + halfThickness, room.center.y },
            { room.size.x, WallThickness, room.size.y }, CeilingColor });

        for (WallSide side : { WallSide::North, WallSide::South, WallSide::East, WallSide::West })
        {
            const bool alongX = side == WallSide::North || side == WallSide::South;
            const float length = alongX ? room.size.x : room.size.y;
            const float depth = alongX ? room.size.y : room.size.x;
            const float sign = side == WallSide::North || side == WallSide::East ? 1.0f : -1.0f;
            const float fixed = sign * (depth * 0.5f + halfThickness);
            const auto AddSegment = [&](float start, float end, float bottom, float top)
            {
                if (end <= start || top <= bottom)
                    return; // 전체 폭/높이 출입구에서는 빈 박스를 생성하지 않는다.
                const float middle = (start + end) * 0.5f;
                const glm::vec3 center = alongX
                    ? glm::vec3(room.center.x + middle, (bottom + top) * 0.5f, room.center.y + fixed)
                    : glm::vec3(room.center.x + fixed, (bottom + top) * 0.5f, room.center.y + middle);
                const glm::vec3 size = alongX
                    ? glm::vec3(end - start, top - bottom, WallThickness)
                    : glm::vec3(WallThickness, top - bottom, end - start);
                m_walls.push_back({ center, size, room.wallColor });
            };

            // E/W 벽을 양끝으로 연장하여 방 모서리의 틈을 닫는다.
            const float cornerExtension = alongX ? 0.0f : WallThickness;
            float cursor = -length * 0.5f - cornerExtension;
            for (const DoorwayDesc& door : openings)
            {
                if (door.side != side)
                    continue;
                const float left = door.offset - door.width * 0.5f;
                const float right = door.offset + door.width * 0.5f;
                AddSegment(cursor, left, 0.0f, room.height);
                AddSegment(left, right, door.height, room.height);
                cursor = right;
            }
            AddSegment(cursor, length * 0.5f + cornerExtension, 0.0f, room.height);
        }
    }
}
