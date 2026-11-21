#pragma once

// 게임 전역 튜닝 값.
// 흩어진 매직 넘버를 한 곳에 모아 밸런스 조정을 쉽게 한다.

namespace ag::config
{
    /* --------------------------- 랜턴 --------------------------- */
    // 랜턴 밝기는 3단계이며, 밝기가 곧 괴물의 감지 거리가 된다.
    

    /* --------------------------- 괴물 --------------------------- */
    

    /* ------------------------- 플레이어 ------------------------- */
    inline constexpr float PitchLimitDeg = 85.0f;
    inline constexpr float MouseSensitivity = 0.10f;    // 도/픽셀
    inline constexpr float PlayerWalkSpeed = 3.0f;      // m/초

    /* --------------------------- 진행 --------------------------- */
    // 레버는 3개이며 정해진 순서대로 올려야 한다. 틀리면 전부 초기화된다.
    inline constexpr int LeverCount = 3;
    inline constexpr int BatteryPickupCount = 5;  // 맵에 배치되는 배터리 수
    inline constexpr int NoteCount = 3;  // 맵에 배치되는 메모 수

    /* --------------------------- 화면 --------------------------- */
    inline constexpr int DefaultWindowWidth  = 1920;
    inline constexpr int DefaultWindowHeight = 1080;
}
