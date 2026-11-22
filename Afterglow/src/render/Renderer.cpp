#include "render/Renderer.h"

#include "render/Camera.h"
#include "render/Mesh.h"
#include "game/Level.h"
#include "game/Lantern.h"
#include "game/Player.h"

namespace ag
{
    namespace
    {
        const glm::vec3 JacketColor{ 0.48f, 0.54f, 0.50f };
        const glm::vec3 SkinColor{ 0.57f, 0.38f, 0.28f };
    }

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

    void Renderer::DrawPlayerBody(const Player& player, const Lantern& lantern)
    {
        const float intensity = lantern.Light().intensity;
        if (intensity <= 0.0f)
            return;

        const glm::vec3 right = player.ViewCamera().Right();
        const glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        const glm::mat4 bodyTransform{
            glm::vec4(right, 0.0f),
            glm::vec4(up, 0.0f),
            glm::vec4(glm::cross(right, up), 0.0f),
            glm::vec4(player.FeetPosition(), 1.0f)
        };

        // 가까운 옷과 신발에 랜턴 빛이 은은하게 퍼지는 효과를 색으로 표현한다.
        m_sceneShader.SetInt("uUnlit", 1);
        const glm::vec3 spill = glm::vec3(0.85f, 0.76f, 0.64f) * intensity;
        const auto drawPart = [&](const glm::vec3& center, const glm::vec3& size, const glm::vec3& color)
        {
            const glm::mat4 model = glm::scale(glm::translate(bodyTransform, center), size);
            DrawMesh(m_cube, model, color * spill);
        };

        const glm::vec3 trousers{ 0.30f, 0.37f, 0.41f };
        const glm::vec3 boots{ 0.29f, 0.25f, 0.22f };

        // 카메라보다 몸통을 살짝 뒤에 두어 고개를 숙였을 때 발끝이 보이게 한다.
        drawPart({ 0.0f, 1.20f, 0.135f }, { 0.45f, 0.50f, 0.16f }, JacketColor);
        drawPart({ -0.26f, 1.30f, 0.105f }, { 0.08f, 0.30f, 0.06f }, JacketColor);
        drawPart({ -0.26f, 1.115f, 0.105f }, { 0.07f, 0.09f, 0.05f }, SkinColor);
        drawPart({ 0.0f, 0.88f, 0.135f }, { 0.45f, 0.16f, 0.16f }, trousers);
        for (float x : { -0.11f, 0.11f })
        {
            drawPart({ x, 0.68f, 0.127f }, { 0.17f, 0.32f, 0.16f }, trousers);
            drawPart({ x, 0.29f, 0.125f }, { 0.14f, 0.49f, 0.17f }, trousers * 0.88f);
            drawPart({ x, 0.075f, 0.02f }, { 0.16f, 0.15f, 0.27f }, boots);
        }
        m_sceneShader.SetInt("uUnlit", 0);
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

        // 오른쪽 재킷 소매와 손잡이를 쥔 손
        drawBox({ 0.25f, -0.12f, -0.31f }, { 0.08f, 0.06f, 0.22f }, JacketColor);
        drawBox({ 0.25f, -0.12f, -0.455f }, { 0.07f, 0.05f, 0.09f }, SkinColor);

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
