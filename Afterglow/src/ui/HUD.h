#pragma once

namespace ag
{
    class Lantern;

    // 플레이 화면 하단 UI: 배터리 잔량, 레버 진행 상태, 현재 랜턴 밝기 단계.
    class HUD
    {
    public:
        bool Initialize();
        void Shutdown();

        void Render(const Lantern& lantern, int raisedLevers, int totalLevers) const;
    };
}
