#include "render/Renderer.h"

#include "render/Camera.h"
#include "render/Mesh.h"
#include "game/Level.h"

namespace ag
{
    bool Renderer::Initialize(int width, int height)
    {
        SetViewport(width, height);
        glDepthFunc(GL_LESS);
        glClearDepth(1.0);
        return m_sceneShader.LoadFromFiles("shaders/scene.vert", "shaders/scene.frag")
            && m_cube.CreateCube(1.0f, 1.0f, 1.0f);
    }

    void Renderer::Shutdown()
    {
        m_cube.Destroy();
        m_sceneShader.Destroy();
    }

    void Renderer::SetViewport(int width, int height)
    {
        m_width  = width;
        m_height = height;
        glViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(const Camera& camera, const Lantern& lantern, const glm::vec3& lanternDirection)
    {
        BeginScene(camera);
    }

    void Renderer::BeginScene(const Camera& camera)
    {
        glEnable(GL_DEPTH_TEST);
        m_sceneShader.Use();

        const float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
        m_sceneShader.SetMat4("view", camera.ViewMatrix());
        m_sceneShader.SetMat4("projection", camera.ProjectionMatrix(aspect));
        m_sceneShader.SetInt("uUnlit", 1);
    }

    void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color)
    {
        m_sceneShader.SetMat4("model", model);
        m_sceneShader.SetVec3("uObjectColor", color);
        mesh.Draw();
    }

    void Renderer::DrawLevel(const Level& level)
    {
        const auto DrawBox = [this](const Wall& box)
        {
            const glm::mat4 model = glm::translate(glm::mat4(1.0f), box.center)
                * glm::scale(glm::mat4(1.0f), box.size);
            DrawMesh(m_cube, model, box.color);
        };
        for (const Wall& surface : level.Surfaces())
            DrawBox(surface);
        for (const Wall& wall : level.Walls())
            DrawBox(wall);
    }

    void Renderer::EndScene()
    {
        glDisable(GL_DEPTH_TEST);
    }
}
