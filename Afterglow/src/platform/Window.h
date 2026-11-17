#pragma once

#include <iostream>

namespace ag
{
    class Input;
    
    class Window
    {
    public:
        bool Create(const char* title, int width, int height, Input* input);
        void Destroy();
        bool ShouldClose() const;
        bool PumpMessages();
        void SwapBuffers();

        int Width() const { return m_width; }
        int Height() const { return m_height; }

    private:
        GLFWwindow* m_handle = nullptr;
        Input* m_input       = nullptr;
        int    m_width       = 0;
        int    m_height      = 0;
    };
}
