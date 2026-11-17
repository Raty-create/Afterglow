#include "platform/Input.h"

namespace ag
{
    void Input::BeginFrame()
    {
        for (int i = 0; i < KeyCount; ++i)
            m_keyPressed[i] = false;
        for (int i = 0; i < MouseButtons; ++i)
            m_mousePressed[i] = false;

        m_mouseDeltaX = 0.0f;
        m_mouseDeltaY = 0.0f;
        m_wheelDelta  = 0.0f;
    }

    void Input::OnKeyDown(int keyCode)
    {
        if (keyCode < 0 || keyCode >= KeyCount)
            return;

        // OS의 키 반복 입력은 "새로 눌림"으로 치지 않는다.
        if (!m_keyDown[keyCode])
            m_keyPressed[keyCode] = true;
        m_keyDown[keyCode] = true;
    }

    void Input::OnKeyUp(int keyCode)
    {
        if (keyCode < 0 || keyCode >= KeyCount)
            return;
        m_keyDown[keyCode] = false;
    }

    void Input::OnMouseMove(int deltaX, int deltaY)
    {
        m_mouseDeltaX += static_cast<float>(deltaX);
        m_mouseDeltaY += static_cast<float>(deltaY);
    }

    void Input::OnMouseWheel(float delta)
    {
        m_wheelDelta += delta;
    }

    void Input::OnMouseButton(int button, bool down)
    {
        if (button < 0 || button >= MouseButtons)
            return;

        if (down && !m_mouseDown[button])
            m_mousePressed[button] = true;
        m_mouseDown[button] = down;
    }

    bool Input::IsKeyDown(int keyCode) const
    {
        if (keyCode < 0 || keyCode >= KeyCount)
            return false;
        return m_keyDown[keyCode];
    }

    bool Input::WasKeyPressed(int keyCode) const
    {
        if (keyCode < 0 || keyCode >= KeyCount)
            return false;
        return m_keyPressed[keyCode];
    }

    bool Input::IsMouseButtonDown(int button) const
    {
        if (button < 0 || button >= MouseButtons)
            return false;
        return m_mouseDown[button];
    }

    bool Input::WasMouseButtonPressed(int button) const
    {
        if (button < 0 || button >= MouseButtons)
            return false;
        return m_mousePressed[button];
    }
}
