#pragma once

#include "render/Shader.h"

namespace ag
{
    class HUD
    {
    public:
        bool Initialize();
        void Shutdown();

        void RenderBattery(float charge, int spareBatteries, int width, int height,
                           float deltaSeconds);

    private:
        Shader m_shader;
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        float m_pulseSeconds = 0.0f;
        float m_displayedCharge = 0.0f;
    };
}
