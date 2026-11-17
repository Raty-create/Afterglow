#pragma once

namespace ag
{
    enum class ScreenState
    {
        Title,    // 타이틀: 시작/종료 안내
        Playing,  // 플레이: 1인칭 진행
        GameOver, // 게임오버: 괴물에게 붙잡힘, 재시작 가능
        Escaped,  // 탈출 성공: 플레이 시간과 사용한 배터리 표시
        Quit      // 종료 요청
    };
}
