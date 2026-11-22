#include "game/Game.h"

#include "platform/Input.h"

namespace ag
{
    namespace
    {
        constexpr std::array TestLeverOrder{ LeverId::Utilities, LeverId::Exhibition, LeverId::Lab };
    }

    bool Game::Initialize()
    {
        m_level.Initialize();
        Reset();
        return true;
    }

    void Game::Reset()
    {
        m_player.Reset(m_level.SpawnPosition());
        m_lantern.Refill();
        m_spareBatteries = 1;
        const auto& batteryPositions = m_level.BatteryPositions();
        for (std::size_t i = 0; i < m_batteries.size(); ++i)
        {
            m_batteries[i].position = batteryPositions[i];
            m_batteries[i].available = true;
        }
        const auto& leverPositions = m_level.LeverPositions();
        m_levers[0] = { LeverId::Exhibition, leverPositions[0], false };
        m_levers[1] = { LeverId::Lab, leverPositions[1], false };
        m_levers[2] = { LeverId::Utilities, leverPositions[2], false };
        m_nextLeverIndex = 0;
        m_escaped        = false;
        m_caught         = false;
    }

    // 갱신 순서를 지킨다.
    //  1) 랜턴 입력(휠) -> 2) 플레이어 이동 -> 3) 랜턴 연료 소모
    //  -> 4) 빛 차폐 판정(level.blocksLight) 후 괴물 갱신 -> 5) 상호작용/레버 판정
    //  -> 6) 접촉 시 m_caught, 문 도달 시 m_escaped 설정
    void Game::Update(float deltaSeconds, const Input& input)
    {
        if (m_escaped || m_caught)
            return;

        UpdateLanternInput(input);
        m_player.Update(deltaSeconds, input, m_level);
        if (m_level.ReachedExit(m_player.FeetPosition()))
        {
            m_escaped = true;
            return;
        }
        UpdateInteraction(input);
        m_lantern.Update(deltaSeconds);
    }

    void Game::UpdateLanternInput(const Input& input)
    {
        // 3단계 밝기 조절
        const float wheel = input.WheelDelta();
        if (wheel > 0.0f)
            m_lantern.StepBrightness(+1);
        else if (wheel < 0.0f)
            m_lantern.StepBrightness(-1);

        // 배터리 교체
        if (input.WasKeyPressed(GLFW_KEY_R) && m_spareBatteries > 0 && m_lantern.Charge() < 1.0f)
        {
            --m_spareBatteries;
            m_lantern.Refill();
        }
    }

    void Game::UpdateInteraction(const Input& input)
    {
        if (!input.WasKeyPressed(GLFW_KEY_F))
            return;

        const auto& camera = m_player.ViewCamera();
        float nearestDistance = 2.0f;
        BatteryPickup* targetBattery = nullptr;
        Lever* targetLever = nullptr;

        const auto isCloserTarget = [&](const glm::vec3& position)
            {
                const glm::vec3 offset = position - camera.Position();
                const float distance = glm::length(offset);
                if (distance <= 0.001f || distance > nearestDistance)
                    return false;

                // 시선 중심에서 약 26도 안에 있는 것만 선택한다.
                if (glm::dot(camera.Forward(), offset / distance) < 0.9f)
                    return false;

                // 벽 뒤에서의 선택은 차단한다.
                if (m_level.BlocksInteraction(camera.Position(), position))
                    return false;

                nearestDistance = distance;
                return true;
            };

        for (auto& battery : m_batteries)
        {
            if (battery.available && isCloserTarget(battery.position))
                targetBattery = &battery;
        }

        for (auto& lever : m_levers)
        {
            if (isCloserTarget(lever.position))
                targetLever = &lever;
        }

        if (targetLever)
            TryActivateLever(*targetLever);
        else if (targetBattery)
        {
            targetBattery->available = false;
            ++m_spareBatteries;
        }
    }

    void Game::TryActivateLever(Lever& lever)
    {
        const auto next = static_cast<std::size_t>(m_nextLeverIndex);
        if (lever.raised || next >= TestLeverOrder.size())
            return;

        if (lever.id == TestLeverOrder[next])
        {
            lever.raised = true;
            ++m_nextLeverIndex;
            return;
        }

        for (auto& item : m_levers)
            item.raised = false;

        m_nextLeverIndex = 0;
    }

    /*bool Game::IsLanternVisibleToMonster() const
    {
        
    }*/

    /*int Game::RaisedLeverCount() const
    {
        
    }*/

    /*int Game::CollectedBatteryCount() const
    {
        
    }*/
}
