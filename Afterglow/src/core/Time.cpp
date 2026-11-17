#include "core/Time.h"

#include <chrono>

namespace
{
    double NowSeconds()
    {
        using clock = std::chrono::steady_clock;
        return std::chrono::duration<double>(clock::now().time_since_epoch()).count();
    }
}

namespace ag
{
    void Time::Reset()
    {
        m_lastCounter = NowSeconds();
        m_delta       = 0.0f;
        m_elapsed     = 0.0f;
    }

    void Time::Tick()
    {
        const double now = NowSeconds();
        const double raw = now - m_lastCounter;

        // 일시정지나 창 드래그로 프레임이 크게 벌어져도 게임이 튀지 않도록 제한한다.
        m_delta = raw > 0.25 ? 0.25f : static_cast<float>(raw);

        m_lastCounter = now;
        m_elapsed += m_delta;
    }
}
