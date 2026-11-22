#include "game/Level.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ag
{
    namespace
    {
        constexpr float WallThickness = 0.2f;
        constexpr float LobbyFloorY = -4.2f; // 기존 연구동 높이 Y=0은 2층이다.
        constexpr float FrontDoorX = 4.5f;
        constexpr float FrontDoorZ = -25.0f;
        constexpr float FrontDoorWidth = 4.0f;
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
        m_fixtures.clear();
        m_collisionOnly.clear();
        m_walkSurfaces.clear();

        // 연구실과 장비실 양쪽에 문을 두어 주 복도와 서비스 복도를 연결한다.
        // 서쪽 기록실과 준비실은 주 복도에서 들어가는 지원 공간이다.
        constexpr float wallHeight = 3.0f;
        constexpr float doorHeight = 2.3f;
        constexpr float labDoorWidth = 2.0f;
        constexpr float supportDoorWidth = 1.6f;
        const RoomDesc startRoom{ { 0.0f, 0.0f }, { 6.0f, 6.0f }, wallHeight };
        const RoomDesc mainHall{ { 0.0f, 24.0f }, { 3.0f, 42.0f }, wallHeight, { 0.42f, 0.49f, 0.51f } };
        const RoomDesc serviceHall{ { 12.75f, 24.0f }, { 2.5f, 42.0f }, wallHeight, { 0.33f, 0.38f, 0.40f } };
        const RoomDesc lab{ { 6.5f, 9.0f }, { 10.0f, 8.0f }, wallHeight, { 0.57f, 0.63f, 0.60f } };
        const RoomDesc records{ { -5.0f, 11.5f }, { 7.0f, 7.0f }, wallHeight, { 0.51f, 0.47f, 0.40f } };
        const RoomDesc prep{ { -5.0f, 21.0f }, { 7.0f, 6.0f }, wallHeight, { 0.43f, 0.51f, 0.54f } };
        const RoomDesc equipment{ { 6.5f, 21.5f }, { 10.0f, 7.0f }, wallHeight, { 0.46f, 0.45f, 0.41f } };
        const RoomDesc optics{ { 6.5f, 31.5f }, { 10.0f, 7.0f }, wallHeight, { 0.45f, 0.49f, 0.53f } };
        const RoomDesc utilities{ { 6.5f, 41.0f }, { 10.0f, 6.0f }, wallHeight, { 0.42f, 0.43f, 0.39f } };
        const RoomDesc computing{ { -5.0f, 31.5f }, { 7.0f, 9.0f }, wallHeight, { 0.45f, 0.48f, 0.54f } };
        const RoomDesc workshop{ { -5.0f, 40.5f }, { 7.0f, 9.0f }, wallHeight, { 0.50f, 0.47f, 0.40f } };
        const RoomDesc stores{ { 18.0f, 18.0f }, { 8.0f, 10.0f }, wallHeight, { 0.47f, 0.44f, 0.37f } };
        const RoomDesc instrumentation{ { 18.0f, 29.5f }, { 8.0f, 13.0f }, wallHeight, { 0.50f, 0.57f, 0.55f } };
        constexpr float eastConnectorOffset = 1.5f;

        AddRoom(startRoom, { { WallSide::North, 0.0f, labDoorWidth, doorHeight } });
        AddRoom(mainHall, {
            { WallSide::South, 0.0f, labDoorWidth, doorHeight },
            { WallSide::East, 4.0f - mainHall.center.y, 1.4f, doorHeight },
            { WallSide::East, lab.center.y - mainHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, equipment.center.y - mainHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, optics.center.y - mainHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, utilities.center.y - mainHall.center.y, labDoorWidth, doorHeight },
            { WallSide::West, records.center.y - mainHall.center.y, supportDoorWidth, doorHeight },
            { WallSide::West, prep.center.y - mainHall.center.y, supportDoorWidth, doorHeight },
            { WallSide::West, computing.center.y - mainHall.center.y, supportDoorWidth, doorHeight },
            { WallSide::West, workshop.center.y - mainHall.center.y, supportDoorWidth, doorHeight }
        });
        AddRoom(serviceHall, {
            { WallSide::West, lab.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::West, equipment.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::West, optics.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::West, utilities.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, stores.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, instrumentation.center.y - serviceHall.center.y, labDoorWidth, doorHeight },
            { WallSide::East, 8.5f - serviceHall.center.y, 1.8f, doorHeight }
        });
        AddRoom(lab, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                       { WallSide::East, 0.0f, labDoorWidth, doorHeight } });
        AddRoom(equipment, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                             { WallSide::East, 0.0f, labDoorWidth, doorHeight } });
        AddRoom(records, { { WallSide::East, 0.0f, supportDoorWidth, doorHeight } });
        AddRoom(prep, { { WallSide::East, 0.0f, supportDoorWidth, doorHeight } });
        AddRoom(optics, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                          { WallSide::East, 0.0f, labDoorWidth, doorHeight } });
        AddRoom(utilities, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                             { WallSide::East, 0.0f, labDoorWidth, doorHeight } });
        AddRoom(computing, { { WallSide::East, 0.0f, supportDoorWidth, doorHeight },
                             { WallSide::North, 0.0f, supportDoorWidth, doorHeight } });
        AddRoom(workshop, { { WallSide::East, 0.0f, supportDoorWidth, doorHeight },
                            { WallSide::South, 0.0f, supportDoorWidth, doorHeight } });
        AddRoom(stores, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                          { WallSide::North, eastConnectorOffset, supportDoorWidth, doorHeight } });
        AddRoom(instrumentation, { { WallSide::West, 0.0f, labDoorWidth, doorHeight },
                                   { WallSide::South, eastConnectorOffset, supportDoorWidth, doorHeight } });

        const glm::vec3 metal{ 0.35f, 0.38f, 0.37f };
        const glm::vec3 worktop{ 0.53f, 0.56f, 0.53f };
        const glm::vec3 filingCabinet{ 0.44f, 0.40f, 0.33f };
        const auto addBench = [&](float x, float z, float width, float depth)
        {
            m_fixtures.push_back({ { x, 0.91f, z }, { width, 0.12f, depth }, worktop });
            for (float side : { -1.0f, 1.0f })
                m_fixtures.push_back({ { x + side * width * 0.36f, 0.425f, z },
                    { 0.16f, 0.85f, depth * 0.78f }, metal });
        };
        addBench(-2.0f, -1.5f, 1.1f, 1.6f); // 시작 방 작업대
        addBench(5.0f, 6.5f, 4.8f, 1.1f);
        addBench(7.8f, 11.5f, 4.8f, 1.1f);  // 연구실 중앙 통로 양쪽
        addBench(-4.8f, 13.8f, 2.2f, 0.8f); // 기록실 책상
        addBench(-5.8f, 23.2f, 3.2f, 0.8f); // 준비실 작업대
        addBench(-6.8f, 28.7f, 2.4f, 0.9f); // 전산실
        addBench(-3.4f, 34.1f, 2.5f, 0.9f);
        addBench(-6.8f, 42.7f, 2.4f, 1.0f); // 정비실
        addBench(5.1f, 29.4f, 4.4f, 0.9f);  // 광학 실험실
        addBench(7.9f, 33.8f, 4.0f, 1.0f);
        addBench(16.2f, 26.1f, 2.9f, 0.9f); // 계측실
        addBench(20.1f, 33.9f, 2.7f, 0.9f);
        m_fixtures.insert(m_fixtures.end(), {
            { { -7.75f, 1.05f, 9.7f }, { 0.7f, 2.1f, 2.3f }, filingCabinet },
            { { -7.75f, 1.05f, 13.3f }, { 0.7f, 2.1f, 2.3f }, filingCabinet },
            { { -7.7f, 0.95f, 19.4f }, { 0.8f, 1.9f, 1.6f }, metal },
            { { 7.8f, 1.05f, 19.2f }, { 3.2f, 2.1f, 0.9f }, metal },
            { { 4.2f, 1.05f, 23.7f }, { 3.2f, 2.1f, 0.9f }, metal },
            { { -7.7f, 1.05f, 33.4f }, { 0.7f, 2.1f, 2.4f }, metal },
            { { -2.35f, 1.05f, 43.6f }, { 0.8f, 2.1f, 1.7f }, metal },
            { { 8.4f, 1.15f, 39.2f }, { 3.0f, 2.3f, 0.95f }, metal },
            { { 4.7f, 1.15f, 42.8f }, { 4.0f, 2.3f, 1.1f }, metal },
            { { 20.95f, 1.0f, 15.5f }, { 0.8f, 2.0f, 2.7f }, filingCabinet },
            { { 20.95f, 1.0f, 20.5f }, { 0.8f, 2.0f, 2.7f }, filingCabinet },
            { { 15.1f, 1.0f, 21.5f }, { 1.2f, 2.0f, 1.8f }, filingCabinet }
        });

        const glm::vec3 instrumentCase{ 0.44f, 0.46f, 0.45f };
        const glm::vec3 instrumentScreen{ 0.18f, 0.35f, 0.30f };
        const auto addInstrument = [&](float x, float z, float front)
        {
            m_fixtures.push_back({ { x, 1.18f, z }, { 0.68f, 0.42f, 0.34f }, instrumentCase });
            m_surfaces.push_back({ { x - 0.09f, 1.21f, z + front * 0.175f },
                { 0.37f, 0.22f, 0.012f }, instrumentScreen });
            m_surfaces.push_back({ { x + 0.18f, 1.18f, z + front * 0.176f },
                { 0.07f, 0.07f, 0.014f }, { 0.62f, 0.53f, 0.34f } });
        };
        // 작업대 위의 계측기 전면은 각 방의 보행 통로를 향한다.
        addInstrument(4.4f, 29.4f, 1.0f);
        addInstrument(6.1f, 29.4f, 1.0f);
        addInstrument(8.2f, 33.8f, -1.0f);
        addInstrument(15.8f, 26.1f, 1.0f);
        addInstrument(16.7f, 26.1f, 1.0f);
        addInstrument(20.2f, 33.9f, -1.0f);

        const glm::vec3 monitorCase{ 0.26f, 0.29f, 0.31f };
        const auto addMonitor = [&](float x, float z, float front)
        {
            m_fixtures.push_back({ { x, 1.05f, z }, { 0.13f, 0.16f, 0.20f }, monitorCase });
            m_fixtures.push_back({ { x, 1.32f, z }, { 0.63f, 0.38f, 0.10f }, monitorCase });
            m_surfaces.push_back({ { x, 1.32f, z + front * 0.052f },
                { 0.55f, 0.30f, 0.008f }, { 0.17f, 0.28f, 0.36f } });
        };
        addMonitor(-7.35f, 28.7f, 1.0f);
        addMonitor(-6.40f, 28.7f, 1.0f);
        addMonitor(-3.4f, 34.1f, -1.0f);
        m_surfaces.push_back({ { 6.5f, 2.75f, 39.2f }, { 9.4f, 0.18f, 0.18f }, metal });
        m_surfaces.push_back({ { 6.5f, 2.75f, 39.55f }, { 9.4f, 0.18f, 0.18f }, worktop });

        const glm::vec3 researchAccent{ 0.30f, 0.56f, 0.56f };
        const glm::vec3 serviceAccent{ 0.64f, 0.49f, 0.25f };
        const glm::vec3 supportAccent{ 0.41f, 0.48f, 0.61f };
        // 복도 바닥 안내선과 각 출입구 위의 구역 표식.
        m_surfaces.push_back({ { 1.05f, 0.003f, 24.0f }, { 0.09f, 0.006f, 40.0f }, researchAccent });
        m_surfaces.push_back({ { 12.05f, 0.003f, 24.0f }, { 0.09f, 0.006f, 40.0f }, serviceAccent });
        const auto markDoor = [&](float x, float z, const glm::vec3& color)
        {
            m_surfaces.push_back({ { x, 2.54f, z }, { 0.04f, 0.18f, 1.45f }, color });
        };
        for (float z : { lab.center.y, optics.center.y })
            markDoor(1.478f, z, researchAccent);
        for (float z : { equipment.center.y, utilities.center.y })
            markDoor(1.478f, z, serviceAccent);
        for (float z : { records.center.y, prep.center.y, computing.center.y, workshop.center.y })
            markDoor(-1.478f, z, supportAccent);
        markDoor(13.978f, stores.center.y, serviceAccent);
        markDoor(13.978f, instrumentation.center.y, researchAccent);

        m_batteryPositions = {{
            { 0.0f, 0.15f, 0.0f },
            { 6.8f, 0.15f, 10.15f },
            { -6.4f, 0.15f, 11.6f },
            { -5.2f, 0.15f, 21.3f },
            { 8.7f, 0.15f, 23.4f },
            { 6.8f, 0.15f, 32.7f },
            { 18.4f, 0.15f, 15.7f },
            { 8.6f, 0.15f, 40.3f }
        }};
        AddLobbyWing();
    }

    void Level::AddFloor(glm::vec2 min, glm::vec2 max, float y, const glm::vec3& color)
    {
        const glm::vec2 center = (min + max) * 0.5f;
        const glm::vec2 size = max - min;
        m_surfaces.push_back({ { center.x, y - WallThickness * 0.5f, center.y },
            { size.x, WallThickness, size.y }, color });
        m_walkSurfaces.push_back({ min, max, y, y });
    }

    void Level::AddLobbyWing()
    {
        constexpr float doorHeight = 2.3f;
        constexpr float balconyY = 0.0f;
        constexpr float balconyHeight = balconyY - LobbyFloorY;
        constexpr float lobbyWest = -8.0f;
        constexpr float lobbyEast = 17.0f;
        constexpr float lobbyNorth = -3.0f;
        constexpr float lobbyCenterZ = (FrontDoorZ + lobbyNorth) * 0.5f;
        constexpr float lobbyHeight = 8.75f;
        constexpr float frontDoorHeight = 3.15f;
        constexpr float linkX = 5.5f;
        constexpr float diningKitchenDoorX = 19.75f;
        const glm::vec3 stone{ 0.42f, 0.44f, 0.43f };
        const glm::vec3 rail{ 0.37f, 0.40f, 0.39f };
        constexpr float railThickness = 0.10f;

        // 1층 정문과 2층 출입구를 하나의 높은 로비 벽에 뚫는다.
        AddRoom({ { FrontDoorX, lobbyCenterZ }, { lobbyEast - lobbyWest, lobbyNorth - FrontDoorZ },
                  lobbyHeight, { 0.62f, 0.61f, 0.55f }, LobbyFloorY }, {
            { WallSide::South, 0.0f, FrontDoorWidth, frontDoorHeight },
            { WallSide::North, linkX - FrontDoorX, 2.4f, doorHeight, balconyHeight },
            { WallSide::West, 2.0f, 2.4f, doorHeight, balconyHeight },
            { WallSide::East, 3.0f, 2.0f, doorHeight, balconyHeight }
        });
        AddRoom({ { -14.0f, -12.5f }, { 12.0f, 15.0f }, 3.0f, { 0.58f, 0.54f, 0.47f } },
            { { WallSide::East, 0.5f, 2.4f, doorHeight } });
        AddRoom({ { 22.0f, -7.0f }, { 10.0f, 20.0f }, 3.0f, { 0.58f, 0.59f, 0.49f } }, {
            { WallSide::West, -4.0f, 2.0f, doorHeight },
            { WallSide::North, diningKitchenDoorX - 22.0f, 2.0f, doorHeight }
        });
        AddRoom({ { 19.0f, 7.75f }, { 10.0f, 9.5f }, 3.0f, { 0.50f, 0.56f, 0.55f } }, {
            { WallSide::South, diningKitchenDoorX - 19.0f, 2.0f, doorHeight },
            { WallSide::West, 0.75f, 1.8f, doorHeight }
        });
        // 시작 방 옆을 지나 기존 주 복도와 이어지는 ㄱ자 연결로.
        AddRoom({ { linkX, 0.0f }, { 4.0f, 6.0f }, 3.0f, { 0.48f, 0.53f, 0.52f } }, {
            { WallSide::South, 0.0f, 2.4f, doorHeight },
            { WallSide::North, 0.0f, 2.0f, doorHeight }
        });
        AddRoom({ { 4.5f, 4.0f }, { 6.0f, 2.0f }, 3.0f, { 0.48f, 0.53f, 0.52f } }, {
            { WallSide::West, 0.0f, 1.4f, doorHeight },
            { WallSide::South, 1.0f, 2.0f, doorHeight }
        });

        constexpr float westEdge = -4.6f;
        constexpr float eastEdge = 13.6f;
        constexpr float stairWest = 1.9f;
        constexpr float stairEast = 7.1f;
        constexpr float stairTopZ = -6.5f;
        constexpr float stairBaseZ = -20.5f;
        constexpr float rearEdge = -21.8f;
        // 2층 난간 통로는 ㄷ자이고, 중앙에서 1층 바닥이 내려다보인다.
        AddFloor({ lobbyWest, stairTopZ }, { lobbyEast, lobbyNorth }, balconyY, stone);
        AddFloor({ lobbyWest, rearEdge }, { westEdge, stairTopZ }, balconyY, stone);
        AddFloor({ eastEdge, rearEdge }, { lobbyEast, stairTopZ }, balconyY, stone);
        const auto addRail = [&](float x0, float z0, float x1, float z1, float y)
        {
            const glm::vec3 center{ (x0 + x1) * 0.5f, y + 0.5f, (z0 + z1) * 0.5f };
            const glm::vec3 size{ std::max(railThickness, x1 - x0), 1.0f,
                                  std::max(railThickness, z1 - z0) };
            // 유리 대신 낮은 난간 벽으로 모델링해 낙하를 막는다.
            m_fixtures.push_back({ center, size, rail });
        };
        addRail(westEdge, rearEdge, westEdge, stairTopZ, balconyY);
        addRail(eastEdge, rearEdge, eastEdge, stairTopZ, balconyY);
        addRail(lobbyWest, rearEdge, westEdge, rearEdge, balconyY);
        addRail(eastEdge, rearEdge, lobbyEast, rearEdge, balconyY);
        addRail(westEdge, stairTopZ, stairWest, stairTopZ, balconyY);
        addRail(stairEast, stairTopZ, eastEdge, stairTopZ, balconyY);

        // 계단의 단을 그리고, 이동 판정에는 그 단을 잇는 경사면을 사용한다.
        constexpr int stairCount = 26;
        constexpr float stairLength = stairTopZ - stairBaseZ;
        constexpr float treadDepth = stairLength / stairCount;
        m_walkSurfaces.push_back({ { stairWest, stairBaseZ }, { stairEast, stairTopZ }, LobbyFloorY, balconyY });
        for (int i = 0; i < stairCount; ++i)
        {
            const float z0 = stairBaseZ + i * treadDepth;
            const float z1 = z0 + treadDepth;
            const float treadY = LobbyFloorY + balconyHeight * (i + 0.5f) / stairCount;
            m_surfaces.push_back({
                { FrontDoorX, (LobbyFloorY + treadY) * 0.5f, (z0 + z1) * 0.5f },
                { stairEast - stairWest, treadY - LobbyFloorY, treadDepth }, stone });
            addRail(stairWest, z0, stairWest, z1, treadY);
            addRail(stairEast, z0, stairEast, z1, treadY);
        }
        // 계단 옆과 뒤의 보조 박스는 충돌에만 사용한다.
        for (const float x : { stairWest, stairEast })
            m_collisionOnly.push_back({ { x, LobbyFloorY + balconyHeight * 0.5f, (stairBaseZ + stairTopZ) * 0.5f },
                                       { railThickness, balconyHeight, stairLength } });
        constexpr float stairBackHeight = balconyHeight - 0.25f;
        m_collisionOnly.push_back({ { FrontDoorX, LobbyFloorY + stairBackHeight * 0.5f, stairTopZ - 0.05f },
                                    { stairEast - stairWest, stairBackHeight, 0.1f } });

        const float halfDoor = FrontDoorWidth * 0.5f;
        AddFloor({ FrontDoorX - halfDoor, FrontDoorZ - 1.0f },
                 { FrontDoorX + halfDoor, FrontDoorZ }, LobbyFloorY, stone);
        const glm::vec3 doorFrame{ 0.37f, 0.42f, 0.40f };
        for (const float side : { -1.0f, 1.0f })
        {
            const float x = FrontDoorX + side * (halfDoor - 0.12f);
            m_fixtures.push_back({ { x, LobbyFloorY + frontDoorHeight * 0.5f, FrontDoorZ },
                                   { 0.20f, frontDoorHeight, 0.24f }, doorFrame });
            // 바깥으로 열린 양개문 사이로 걸어 나갈 수 있다.
            m_fixtures.push_back({ { x, LobbyFloorY + (frontDoorHeight - 0.1f) * 0.5f, FrontDoorZ - 0.53f },
                                   { 0.08f, frontDoorHeight - 0.1f, 0.98f }, doorFrame });
        }
        m_surfaces.push_back({ { FrontDoorX, LobbyFloorY + frontDoorHeight + 0.07f, FrontDoorZ + 0.10f },
                               { FrontDoorWidth - 0.20f, 0.15f, 0.15f }, doorFrame });
        m_surfaces.push_back({ { FrontDoorX, LobbyFloorY + frontDoorHeight + 0.25f, FrontDoorZ + 0.035f },
                               { 0.95f, 0.17f, 0.07f }, { 0.34f, 0.63f, 0.46f } });

        const glm::vec3 cabinet{ 0.44f, 0.43f, 0.37f };
        const glm::vec3 display{ 0.45f, 0.52f, 0.55f };
        // 중앙 통로는 비워 두고, 열주와 안내 데스크, 좌석을 양옆에 배치한다.
        for (const float x : { -2.8f, 11.8f })
            for (const float z : { -9.4f, -17.8f })
            {
                m_fixtures.push_back({ { x, LobbyFloorY + lobbyHeight * 0.5f, z },
                                       { 0.76f, lobbyHeight, 0.76f }, stone });
                m_fixtures.push_back({ { x, LobbyFloorY + 0.16f, z },
                                       { 1.05f, 0.32f, 1.05f }, rail });
            }
        m_fixtures.push_back({ { 11.8f, LobbyFloorY + 0.5f, -22.0f }, { 6.0f, 1.0f, 1.15f }, cabinet });
        m_fixtures.push_back({ { 11.8f, LobbyFloorY + 1.04f, -22.0f }, { 6.2f, 0.10f, 1.35f }, stone });
        const glm::vec3 upholstery{ 0.32f, 0.29f, 0.27f };
        for (const float z : { -11.8f, -18.2f })
        {
            m_fixtures.push_back({ { -5.2f, LobbyFloorY + 0.31f, z }, { 2.7f, 0.62f, 0.95f }, upholstery });
            m_fixtures.push_back({ { -5.2f, LobbyFloorY + 0.68f, z + 0.38f }, { 2.7f, 0.62f, 0.19f }, upholstery });
            m_fixtures.push_back({ { -5.2f, LobbyFloorY + 0.25f, z - 1.35f }, { 1.5f, 0.50f, 0.65f }, cabinet });
        }
        for (const float z : { -10.2f, -16.4f })
            m_surfaces.push_back({ { FrontDoorX, LobbyFloorY + lobbyHeight - 0.20f, z },
                                   { 7.4f, 0.4f, 0.35f }, rail });

        for (const float x : { -17.0f, -13.2f, -9.5f })
        {
            m_fixtures.push_back({ { x, 0.45f, -16.0f }, { 1.9f, 0.9f, 1.15f }, cabinet });
            m_fixtures.push_back({ { x, 1.16f, -16.0f }, { 1.75f, 0.52f, 1.0f }, display });
        }
        for (const float x : { 19.35f, 24.65f })
            for (const float z : { -14.0f, -7.5f, -2.7f, 0.8f })
                m_fixtures.push_back({ { x, 0.4f, z }, { 2.2f, 0.8f, 1.3f }, cabinet });
        m_fixtures.push_back({ { 22.8f, 0.52f, 7.1f }, { 1.0f, 1.04f, 6.0f }, rail });
        m_fixtures.push_back({ { 19.0f, 0.52f, 5.1f }, { 2.4f, 1.04f, 0.95f }, rail });
    }

    void Level::AddRoom(const RoomDesc& room, const std::vector<DoorwayDesc>& doors)
    {
        if (!IsPositive(room.size.x) || !IsPositive(room.size.y) || !IsPositive(room.height)
            || !std::isfinite(room.floorY)
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
                || !std::isfinite(door.bottom) || door.bottom < 0.0f
                || door.bottom + door.height > room.height || door.width > length
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
        AddFloor(room.center - room.size * 0.5f, room.center + room.size * 0.5f, room.floorY, FloorColor);
        m_surfaces.push_back({ { room.center.x, room.floorY + room.height + halfThickness, room.center.y },
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
                const float centerY = room.floorY + (bottom + top) * 0.5f;
                const glm::vec3 center = alongX
                    ? glm::vec3(room.center.x + middle, centerY, room.center.y + fixed)
                    : glm::vec3(room.center.x + fixed, centerY, room.center.y + middle);
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
                AddSegment(left, right, 0.0f, door.bottom);
                AddSegment(left, right, door.bottom + door.height, room.height);
                cursor = right;
            }
            AddSegment(cursor, length * 0.5f + cornerExtension, 0.0f, room.height);
        }
    }

    bool Level::TryGetFloor(glm::vec2 position, float currentY, float& floorY) const
    {
        constexpr float maxStepHeight = 0.23f;
        bool found = false;
        for (const auto& surface : m_walkSurfaces)
        {
            if (position.x < surface.min.x || position.x > surface.max.x
                || position.y < surface.min.y || position.y > surface.max.y)
                continue;
            const float t = (position.y - surface.min.y) / (surface.max.y - surface.min.y);
            const float y = glm::mix(surface.southY, surface.northY, t);
            const float distance = std::abs(y - currentY);
            // 계단 첫 단과 아래층 바닥이 겹치면 오를 수 있는 높은 쪽을 택한다.
            if (distance < maxStepHeight && (!found || y > floorY))
            {
                floorY = y;
                found = true;
            }
        }
        return found;
    }

    bool Level::ReachedExit(const glm::vec3& feet) const
    {
        return std::abs(feet.y - LobbyFloorY) < 0.05f
            && std::abs(feet.x - FrontDoorX) < FrontDoorWidth * 0.5f
            && feet.z < FrontDoorZ - 0.25f;
    }

    bool Level::IntersectsWall(const glm::vec3& center, const glm::vec3& halfSize) const
    {
        const auto overlaps = [&](const Wall& obstacle)
        {
            const glm::vec3 distance = glm::abs(center - obstacle.center);
            const glm::vec3 reach = halfSize + obstacle.size * 0.5f;
            return distance.x < reach.x && distance.y < reach.y && distance.z < reach.z;
        };
        return std::any_of(m_walls.begin(), m_walls.end(), overlaps)
            || std::any_of(m_fixtures.begin(), m_fixtures.end(), overlaps)
            || std::any_of(m_collisionOnly.begin(), m_collisionOnly.end(), overlaps);
    }
}
