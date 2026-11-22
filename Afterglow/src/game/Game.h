#pragma once

#include "core/Config.h"
#include "game/BatteryPickup.h"
#include "game/ExitDoor.h"
#include "game/Lantern.h"
#include "game/Lever.h"
#include "game/Level.h"
#include "game/Monster.h"
#include "game/Note.h"
#include "game/Player.h"

#include <array>

namespace ag
{
    class Input;

    // 게임 규칙 전체를 총괄한다.
    //  - 레버 3개를 정해진 순서대로 올리면 탈출문이 열린다. 순서를 틀리면 전부 초기화된다.
    //  - 랜턴 밝기에서 계산한 감지 거리로 괴물이 빛을 감지한다.
    //  - 괴물에게 붙잡히면 게임오버, 문에 도달하면 탈출 성공.
    class Game
    {
    public:
        bool Initialize();
        void Reset();

        // 한 프레임 진행. 결과는 IsCaught()/IsEscaped() 로 확인해 화면 상태를 바꾼다.
        void Update(float deltaSeconds, const Input& input);

        const ag::Level&   Level() const { return m_level; }
        const ag::Player&  Player() const { return m_player; }
        const ag::Monster& Monster() const { return m_monster; }
        const ag::Lantern& Lantern() const { return m_lantern; }
        const auto& Batteries() const { return m_batteries; }

        int  RaisedLeverCount() const;
        int  CollectedBatteryCount() const;
        bool IsEscaped() const { return m_escaped; }
        bool IsCaught() const { return m_caught; }

    private:
        void UpdateLanternInput(const Input& input);
        void UpdateInteraction(const Input& input);
        void UpdateLevers(const Input& input);
        bool IsLanternVisibleToMonster() const;

        ag::Level   m_level;
        ag::Player  m_player;
        ag::Monster m_monster;
        ag::Lantern m_lantern;

        std::array<Lever, config::LeverCount> m_levers{};
        std::array<BatteryPickup, config::BatteryPickupCount> m_batteries{};
        std::array<Note, config::NoteCount> m_notes{};
        ExitDoor m_exitDoor;

        int  m_spareBatteries = 0;
        int  m_nextLeverIndex = 0;   // 다음에 올려야 하는 레버 번호
        bool m_escaped        = false;
        bool m_caught         = false;
    };
}
