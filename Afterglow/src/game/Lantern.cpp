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

        if (m_charge <= 0.0f)
            return { 0.0f, 0.0f };

        LightSettings light = levels[m_brightnessIndex];
        light.intensity *= m_charge;
        light.range *= std::sqrtf(m_charge);
        return light;
    }

    void Lantern::Update(float deltaSeconds)
    {
        // 꺼짐, 약, 중, 강의 초당 소모량
        constexpr float drainPerSecond[] = { 0.0f, 0.002f, 0.007f, 0.01f };
        m_charge = std::max(0.0f, m_charge - drainPerSecond[m_brightnessIndex] * deltaSeconds);
    }

    float Lantern::Charge() const
    {
        return m_charge;
    }

    void Lantern::Refill()
    {
        m_charge = 1.0f;
    }
}
