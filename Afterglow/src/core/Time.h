#pragma once

namespace ag
{
    // 프레임 간 시간과 총 플레이 시간을 관리한다.
    // 탈출 성공 화면의 플레이 시간 표시에도 사용한다.
    class Time
    {
    public:
        void Reset();      // 타이머 기준점 초기화
        void Tick();       // 프레임 시작 시 호출: delta 갱신

        float DeltaSeconds() const { return m_delta; }
        float ElapsedSeconds() const { return m_elapsed; }

    private:
        double m_lastCounter = 0.0;
        float  m_delta       = 0.0f;
        float  m_elapsed     = 0.0f;
    };
}
