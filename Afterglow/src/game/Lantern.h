#pragma once

#include "core/Config.h"

namespace ag
{
    // 랜턴은 조명이면서 동시에 게임 규칙이다.
    // 밝기 단계(4단계)가 곧 괴물의 감지 거리가 되고, 연료가 떨어지면 배터리를 교체해야 한다.
    class Lantern
    {
    public:
        struct LightSettings
        {
            float intensity;
            float range;
        };

        void StepBrightness(int direction);
        LightSettings Light() const;

        void Update(float deltaSeconds);

        void Charge();
        void Refill();

    private:
        int   m_brightnessIndex = 2;
        float m_charge = 1.0f;
    };
}
