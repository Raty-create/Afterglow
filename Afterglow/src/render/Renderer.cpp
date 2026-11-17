#include "render/Renderer.h"

#include "render/Camera.h"
#include "render/Mesh.h"

namespace ag
{
    bool Renderer::Initialize(int width, int height)
    {
        SetViewport(width, height);
        glDepthFunc(GL_LESS);
        glClearDepth(1.0);
        return m_sceneShader.LoadFromFiles("shaders/scene.vert", "shaders/scene.frag");
    }

    void Renderer::Shutdown()
    {
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
        glEnable(GL_DEPTH_TEST);
        m_sceneShader.Use();

        const float aspect = static_cast<float>((m_width) / m_height);
        m_sceneShader.SetMat4("view", camera.ViewMatrix());
        m_sceneShader.SetMat4("projection", camera.ProjectionMatrix(aspect));
        m_sceneShader.SetInt("uUnlit", 1);
    }

    void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color)
    {
        m_sceneShader.SetMat4("model", model);
        mesh.Draw();
    }

    void Renderer::EndScene()
    {
        glDisable(GL_DEPTH_TEST);
    }
}
