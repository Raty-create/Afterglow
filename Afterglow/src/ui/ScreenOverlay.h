#pragma once

namespace ag
{
    // 플레이 이외의 화면(타이틀/게임오버/탈출 성공)에 표시되는 2D 오버레이.
    class ScreenOverlay
    {
    public:
        bool Initialize();
        void Shutdown();

        void RenderTitle() const;
        void RenderGameOver() const;
        void RenderEscaped(float elapsedSeconds, int batteriesUsed) const;
    };
}
