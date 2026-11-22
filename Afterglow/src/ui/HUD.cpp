#include "ui/HUD.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace ag
{
    bool HUD::Initialize()
    {
        if (!m_shader.LoadFromFiles("shaders/ui.vert", "shaders/ui.frag"))
            return false;

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        if (m_vao == 0 || m_vbo == 0)
        {
            Shutdown();
            return false;
        }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }

    void HUD::Shutdown()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        m_vbo = 0;
        m_vao = 0;
        m_displayedCharge = 0.0f;
        m_shader.Destroy();
    }

    void HUD::RenderBattery(float charge, int spareBatteries, int width, int height, float deltaSeconds)
    {
        if (width <= 0 || height <= 0 || !m_shader.IsValid())
            return;

        charge = std::clamp(charge, 0.0f, 1.0f);
        const int iconCount = std::max(1, spareBatteries);
        const float contentWidth = std::max(172.0f, 24.0f * iconCount - 5.0f);
        const float scale = std::min({ height / 900.0f, width / (contentWidth + 48.0f), 2.0f });
        const float toNdcX = 2.0f * scale / width;
        const float toNdcY = 2.0f * scale / height;

        constexpr float fillLeft = 34.0f;
        constexpr float fillWidth = 168.0f;
        if (charge >= m_displayedCharge)
        {
            m_displayedCharge = charge;
        }
        else
        {
            constexpr float responsePerSecond = 10.0f;
            const float blend = -std::expm1(-responsePerSecond * deltaSeconds);
            m_displayedCharge = glm::mix(m_displayedCharge, charge, blend);
            if (charge == 0.0f && m_displayedCharge * fillWidth * scale < 0.05f)
                m_displayedCharge = 0.0f;
        }

        m_shader.Use();
        m_shader.SetInt("uUseTexture", 0);
        m_shader.SetFloat("uFillRightPx", 0.0f);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // 좌하단을 원점으로 한 UI 좌표를 NDC로 변환한다.
        const auto drawRect = [&](float x, float y, float w, float h, const glm::vec4& color)
        {
            const float left = x * toNdcX - 1.0f;
            const float right = (x + w) * toNdcX - 1.0f;
            const float bottom = y * toNdcY - 1.0f;
            const float top = (y + h) * toNdcY - 1.0f;
            const glm::vec2 vertices[] = { { left, bottom }, { right, bottom }, { left, top }, { right, top } };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            m_shader.SetVec4("uColor", color);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        };

        const auto drawOutline = [&](float x, float y, float w, float h, const glm::vec4& color)
        {
            constexpr float edge = 2.0f;
            drawRect(x, y, w, edge, color);
            drawRect(x, y + h - edge, w, edge, color);
            drawRect(x, y + edge, edge, h - 2.0f * edge, color);
            drawRect(x + w - edge, y + edge, edge, h - 2.0f * edge, color);
        };

        const glm::vec4 outline{ 0.42f, 0.42f, 0.41f, 1.0f };
        const glm::vec4 normal{ 0.64f, 0.64f, 0.62f, 1.0f };
        const glm::vec4 yellow{ 0.56f, 0.48f, 0.23f, 1.0f };
        const glm::vec4 orange{ 0.63f, 0.35f, 0.20f, 1.0f };
        const glm::vec4 dimRed{ 0.33f, 0.11f, 0.12f, 1.0f };
        const glm::vec4 brightRed{ 0.66f, 0.19f, 0.18f, 1.0f };
        constexpr float pulseCharge = 0.20f;
        glm::vec4 batteryOutline;
        glm::vec4 batteryFill;
        if (charge <= pulseCharge)
        {
            const float pulse = 0.5f + 0.5f * std::cos(2.0f * std::numbers::pi_v<float> * m_pulseSeconds);
            batteryFill = glm::mix(dimRed, brightRed, pulse);
            batteryOutline = batteryFill;
            m_pulseSeconds = std::fmod(m_pulseSeconds + deltaSeconds, 1.0f);
        }
        else
        {
            m_pulseSeconds = 0.0f;

            // 각 색은 지정된 잔량 구간에서만 보간된다.
            const float toYellow = 1.0f - glm::smoothstep(0.40f, 0.45f, charge);
            const float toOrange = 1.0f - glm::smoothstep(0.30f, 0.35f, charge);
            const float toRed = 1.0f - glm::smoothstep(pulseCharge, 0.25f, charge);
            const auto chargeColor = [&](const glm::vec4& gray)
            {
                const glm::vec4 yellowBlend = glm::mix(gray, yellow, toYellow);
                const glm::vec4 orangeBlend = glm::mix(yellowBlend, orange, toOrange);
                return glm::mix(orangeBlend, brightRed, toRed);
            };
            batteryFill = chargeColor(normal);
            batteryOutline = chargeColor(outline);
        }

        drawOutline(32.0f, 50.0f, 172.0f, 20.0f, batteryOutline);
        drawRect(204.0f, 56.0f, 4.0f, 8.0f, batteryOutline);
        if (m_displayedCharge > 0.0f)
        {
            // 끝 픽셀은 채워진 비율만큼 합성해 1픽셀 미만의 움직임도 표현한다.
            m_shader.SetFloat("uFillRightPx", (fillLeft + fillWidth * m_displayedCharge) * scale);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            drawRect(fillLeft, 52.0f, fillWidth, 16.0f, batteryFill);
            glDisable(GL_BLEND);
            m_shader.SetFloat("uFillRightPx", 0.0f);
        }

        for (int i = 0; i < iconCount; ++i)
        {
            const float x = 32.0f + 24.0f * i;
            const bool hasSpare = i < spareBatteries;
            if (hasSpare)
                drawRect(x, 32.0f, 16.0f, 11.0f, normal);
            else
                drawOutline(x, 32.0f, 16.0f, 11.0f, outline);
            drawRect(x + 16.0f, 35.0f, 3.0f, 5.0f, hasSpare ? normal : outline);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
