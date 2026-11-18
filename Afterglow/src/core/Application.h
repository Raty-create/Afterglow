#pragma once

#include "core/ScreenState.h"
#include "core/Time.h"
#include "platform/Input.h"
#include "render/Camera.h"

namespace ag
{
    class Window;
    class Renderer;
    class Game;
    class HUD;
    class ScreenOverlay;

    // 프로그램 전체 수명 주기를 관리한다.
    // 플랫폼(창/입력) -> 게임 로직 -> 렌더링 순으로 한 프레임을 진행한다.
    class Application
    {
    public:
        Application();
        ~Application();

        bool Initialize();
        void Run();
        void Shutdown();

        ScreenState Screen() const { return m_screen; }
        void SetScreen(ScreenState next) { m_screen = next; }

    private:
        void Update(float deltaSeconds);
        void Render();

        Window*         m_window  = nullptr;
        Renderer*       m_renderer = nullptr;
        Game*           m_game    = nullptr;
        HUD*            m_hud     = nullptr;
        ScreenOverlay*  m_overlay = nullptr;

        Input           m_input;
        Time            m_time;
        Camera          m_mapCamera; // 이동 구현 전 맵 확인용 고정 시점.
        ScreenState     m_screen  = ScreenState::Title;
        bool            m_running = false;
    };
}
