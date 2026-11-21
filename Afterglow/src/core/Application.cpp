#include "core/Application.h"

#include "core/Config.h"
#include "game/Game.h"
#include "platform/Window.h"
#include "render/Renderer.h"
#include "ui/HUD.h"
#include "ui/ScreenOverlay.h"

namespace ag
{
    Application::Application() = default;
    Application::~Application() = default;

    bool Application::Initialize()
    {
        m_window = new Window();
        if (!m_window->Create("잔광 (Afterglow)", config::DefaultWindowWidth, config::DefaultWindowHeight, &m_input))
            return false;

        m_renderer = new Renderer();
        if (!m_renderer->Initialize(m_window->Width(), m_window->Height()))
            return false;

        m_game = new Game();
        m_hud  = new HUD();
        m_overlay = new ScreenOverlay();

        if (!m_game->Initialize())
            return false;

        m_time.Reset();
        m_screen  = ScreenState::Playing;
        m_window->CaptureCursor();
        m_running = true;
        return true;
    }

    void Application::Run()
    {
        while (m_running)
        {
            m_input.BeginFrame();

            const int oldWidth = m_window->Width();
            const int oldHeight = m_window->Height();
            if (!m_window->PumpMessages())
                break;

            m_time.Tick();
            Update(m_time.DeltaSeconds());
            if (!m_running)
                break;

            const int width = m_window->Width();
            const int height = m_window->Height();
            if (width == 0 || height == 0)
                continue;

            if (width != oldWidth || height != oldHeight)
                m_renderer->SetViewport(width, height);

            Render();
            m_window->SwapBuffers();
        }
    }

    void Application::Update(float deltaSeconds)
    {
        if (m_input.WasKeyPressed(GLFW_KEY_ESCAPE))
        {
            m_running = false;
            return;
        }

        if (m_screen == ScreenState::Playing)
            m_game->Update(deltaSeconds, m_input);
    }

    void Application::Render()
    {
        glClearColor(0.08f, 0.10f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_renderer->BeginScene(m_game->Player().ViewCamera());
        m_renderer->DrawLevel(m_game->Level());
        m_renderer->DrawHeldLantern();
        m_renderer->EndScene();
    }

    void Application::Shutdown()
    {
        m_running = false;

        delete m_overlay;
        delete m_hud;
        delete m_game;

        if (m_renderer)
            m_renderer->Shutdown();
        delete m_renderer;

        if (m_window)
            m_window->Destroy();
        delete m_window;

        m_overlay  = nullptr;
        m_hud      = nullptr;
        m_game     = nullptr;
        m_renderer = nullptr;
        m_window   = nullptr;
    }
}
