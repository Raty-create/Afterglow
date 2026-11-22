#include "game/Lantern.h"

#include <algorithm>

namespace ag
{
    void Lantern::StepBrightness(int direction)
    {
        m_brightnessIndex = std::clamp(m_brightnessIndex + direction, 0, 3);
    }

    Lantern::LightSettings Lantern::Light() const
    {
        constexpr LightSettings levels[] =
        {
            { 0.00f, 0.00f },   // 꺼짐
            { 0.45f,  7.0f },   // 약
            { 0.65f, 10.0f },   // 중
            { 1.00f, 18.0f }    // 강
        };

        return levels[m_brightnessIndex];
    }
}
