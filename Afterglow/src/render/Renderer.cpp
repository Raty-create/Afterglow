#include "render/Renderer.h"

#include "render/Camera.h"
#include "render/Mesh.h"
#include "game/Level.h"
#include "game/Lantern.h"

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

        const auto light = lantern.Light();
        m_sceneShader.SetInt("uUnlit", 0);
        m_sceneShader.SetInt("uLightOn", light.intensity > 0.0f ? 1 : 0);
        m_sceneShader.SetVec3("uLightPos", camera.Position());
        m_sceneShader.SetVec3("uLightDir", lanternDirection);
        m_sceneShader.SetVec3("uLightColor", { 1.0f, 0.92f, 0.78f });
        m_sceneShader.SetVec3("uEyeDir", -camera.Forward());
        m_sceneShader.SetFloat("uLightRange", light.range);
        m_sceneShader.SetFloat("uLightIntensity", light.intensity);
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

    void Renderer::DrawHeldLantern(const Lantern& lantern)
    {
        const float intensity = lantern.Light().intensity;
        if (intensity <= 0.0f)
            return;

        // 맵의 깊이만 지운다. 손과 랜턴 부품끼리는 깊이 테스트를 한다.
        glClear(GL_DEPTH_BUFFER_BIT);

        m_sceneShader.SetInt("uUnlit", 1);

        // 카메라 공간에서 +X는 오른쪽, +Y는 위, -Z는 앞이다.
        // BeginScene()에서 설정한 투영 행렬은 그래도 사용한다.
        m_sceneShader.SetMat4("view", glm::mat4(1.0f));

        const auto drawBox = [this, intensity](const glm::vec3& center, const glm::vec3& size, const glm::vec3& color)
            {
                const glm::mat4 model = glm::translate(glm::mat4(1.0f), center)
                    * glm::scale(glm::mat4(1.0f), size);
                DrawMesh(m_cube, model, color * intensity);
            };

        // 손과 손목
        drawBox({ 0.25f, -0.12f, -0.35f }, { 0.07f, 0.05f, 0.30f }, { 0.57f, 0.38f, 0.28f });

        // 랜턴 손잡이
        drawBox({ 0.25f, -0.12f, -0.48f }, { 0.10f, 0.01f, 0.01f }, { 0.25f, 0.23f, 0.24f });
        drawBox({ 0.20f, -0.15f, -0.48f }, { 0.01f, 0.07f, 0.01f }, { 0.25f, 0.23f, 0.24f });
        drawBox({ 0.30f, -0.15f, -0.48f }, { 0.01f, 0.07f, 0.01f }, { 0.25f, 0.23f, 0.24f });

        // 랜턴 몸체
        drawBox({ 0.25f, -0.18f, -0.48f }, { 0.12f, 0.03f, 0.12f }, { 0.21f, 0.23f, 0.24f });
        drawBox({ 0.25f, -0.33f, -0.48f }, { 0.12f, 0.03f, 0.12f }, { 0.21f, 0.23f, 0.24f });

        // 카메라에서 보이는 임시 발광부
        drawBox({ 0.25f, -0.255f, -0.48f }, { 0.09f, 0.12f, 0.09f }, { 1.00f, 0.75f, 0.35f });
    }

    void Renderer::EndScene()
    {
        glDisable(GL_DEPTH_TEST);
    }
}
