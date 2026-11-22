#include "game/Game.h"

#include "platform/Input.h"

namespace ag
{
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
        BatteryPickup* target = nullptr;
        float nearestDistance = 2.0f;

        for (auto& battery : m_batteries)
        {
            if (!battery.available)
                continue;

            const glm::vec3 offset = battery.position - camera.Position();
            const float distance = glm::length(offset);
            if (distance <= 0.001f || distance > nearestDistance)
                continue;

            // 시선 중심에서 약 26도 안에 있는 배터리만 선택한다.
            if (glm::dot(camera.Forward(), offset / distance) < 0.9f)
                continue;

            target = &battery;
            nearestDistance = distance;
        }

        if (target)
        {
            target->available = false;
            ++m_spareBatteries;
        }
    }

    void Game::UpdateLevers(const Input& input)
    {
        
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
