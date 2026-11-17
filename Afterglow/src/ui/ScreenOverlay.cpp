#include "ui/ScreenOverlay.h"

namespace ag
{
    bool ScreenOverlay::Initialize()
    {
        // 전체 화면 쿼드/텍스트 준비.
        return false;
    }

    void ScreenOverlay::Shutdown()
    {
        // 자원 해제.
    }

    void ScreenOverlay::RenderTitle() const
    {
        // 어두운 배경 + 깜빡이는 랜턴 불빛, 시작(Enter/클릭) 안내, 종료 안내.
    }

    void ScreenOverlay::RenderGameOver() const
    {
        // 붉게 암전되는 연출 후 재시작(R) 안내.
    }

    void ScreenOverlay::RenderEscaped(float /*elapsedSeconds*/, int /*batteriesUsed*/) const
    {
        // 밝아지는 화면과 함께 플레이 시간/사용한 배터리 수 표시.
    }
}
