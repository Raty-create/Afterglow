#include "ui/HUD.h"

#include "game/Lantern.h"

namespace ag
{
    bool HUD::Initialize()
    {
        // 2D 쿼드/텍스트 배치용 버퍼 준비.
        return false;
    }

    void HUD::Shutdown()
    {
        // 자원 해제.
    }

    void HUD::Render(const Lantern& /*lantern*/, int /*raisedLevers*/, int /*totalLevers*/) const
    {
        // 배터리 잔량 바, 레버 3칸 진행 표시, 밝기 3단계 아이콘을 그린다.
        // 배터리가 일정 이하이면 깜빡이는 경고를 표시한다.
    }
}
