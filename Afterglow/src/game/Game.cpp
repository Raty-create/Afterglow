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
    }

    void Game::UpdateLanternInput(const Input& input)
    {
        const float wheel = input.WheelDelta();
        if (wheel > 0.0f)
            m_lantern.StepBrightness(+1);
        else if (wheel < 0.0f)
            m_lantern.StepBrightness(-1);
    }

    void Game::UpdateInteraction(const Input& input)
    {
        
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
