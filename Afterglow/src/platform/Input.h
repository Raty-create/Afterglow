#pragma once

namespace ag
{
    // 키보드/마우스 상태를 프레임 단위로 보관한다.
    // 플랫폼 계층(Window)이 이벤트를 on* 함수로 전달하고, 게임 로직은 is*/was* 로 조회한다.
    class Input
    {
    public:
        void BeginFrame();   // 프레임 시작: 이동량/눌림 이벤트 초기화

        /* --- 플랫폼 계층에서 호출 --- */
        void OnKeyDown(int keyCode);
        void OnKeyUp(int keyCode);
        void OnMouseMove(int deltaX, int deltaY);
        void OnMouseWheel(float delta);
        void OnMouseButton(int button, bool down);

        /* --- 게임 로직에서 조회 --- */
        bool IsKeyDown(int keyCode) const;
        bool WasKeyPressed(int keyCode) const;    // 이번 프레임에 새로 눌림

        float MouseDeltaX() const { return m_mouseDeltaX; }
        float MouseDeltaY() const { return m_mouseDeltaY; }
        float WheelDelta() const { return m_wheelDelta; }

        bool IsMouseButtonDown(int button) const;
        bool WasMouseButtonPressed(int button) const;

        static constexpr int KeyCount = GLFW_KEY_LAST + 1;
        static constexpr int MouseButtons = 3;

    private:
        bool m_keyDown[KeyCount] = {};
        bool m_keyPressed[KeyCount] = {};
        bool m_mouseDown[MouseButtons] = {};
        bool m_mousePressed[MouseButtons] = {};

        float m_mouseDeltaX = 0.0f;
        float m_mouseDeltaY = 0.0f;
        float m_wheelDelta  = 0.0f;
    };
}
