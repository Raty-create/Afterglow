#include "render/Renderer.h"

#include "game/BatteryPickup.h"
#include "render/BoxSurface.h"
#include "render/Camera.h"
#include "render/Mesh.h"
#include "game/Level.h"
#include "game/Lantern.h"
#include "game/Lever.h"
#include "game/Player.h"

namespace ag
{
    namespace
    {
        const glm::vec3 JacketColor{ 0.48f, 0.54f, 0.50f };
        const glm::vec3 SkinColor{ 0.57f, 0.38f, 0.28f };

        // 부품이 겹치는 손잡이와 양극 표시도 레벨 구조물과 같은 방식으로 합친다.
        const Wall BatteryParts[] = {
            { { 0.0f, -0.025f, 0.0f }, { 0.14f, 0.25f, 0.14f }, { 0.12f, 0.55f, 0.47f } },
            { { 0.0f, 0.1125f, 0.0f }, { 0.14f, 0.025f, 0.14f }, { 0.95f, 0.60f, 0.18f } },
            { { 0.0f, 0.1375f, 0.0f }, { 0.06f, 0.025f, 0.06f }, { 0.85f, 0.86f, 0.88f } },
            { { 0.0f, 0.025f, -0.071f }, { 0.066f, 0.014f, 0.004f }, { 0.94f, 0.95f, 0.88f } },
            { { 0.0f, 0.025f, -0.071f }, { 0.014f, 0.066f, 0.004f }, { 0.94f, 0.95f, 0.88f } }
        };
        const Wall HeldLanternParts[] = {
            { { 0.25f, -0.12f, -0.31f }, { 0.08f, 0.06f, 0.22f }, JacketColor },
            { { 0.25f, -0.12f, -0.455f }, { 0.07f, 0.05f, 0.09f }, SkinColor },
            { { 0.25f, -0.12f, -0.48f }, { 0.10f, 0.01f, 0.01f }, { 0.25f, 0.23f, 0.24f } },
            { { 0.20f, -0.15f, -0.48f }, { 0.01f, 0.07f, 0.01f }, { 0.25f, 0.23f, 0.24f } },
            { { 0.30f, -0.15f, -0.48f }, { 0.01f, 0.07f, 0.01f }, { 0.25f, 0.23f, 0.24f } },
            { { 0.25f, -0.18f, -0.48f }, { 0.12f, 0.03f, 0.12f }, { 0.21f, 0.23f, 0.24f } },
            { { 0.25f, -0.33f, -0.48f }, { 0.12f, 0.03f, 0.12f }, { 0.21f, 0.23f, 0.24f } },
            { { 0.25f, -0.255f, -0.48f }, { 0.09f, 0.12f, 0.09f }, { 1.00f, 0.75f, 0.35f } }
        };
        const Wall LeverHousingParts[] = {
            { { 0.0f, 0.0f, 0.08f }, { 0.44f, 0.64f, 0.09f }, { 0.36f, 0.39f, 0.39f } },
            { { 0.0f, 0.0f, 0.03f }, { 0.36f, 0.53f, 0.055f }, { 0.55f, 0.56f, 0.48f } },
            { { 0.0f, 0.0f, 0.0f }, { 0.16f, 0.16f, 0.08f }, { 0.23f, 0.26f, 0.26f } }
        };
        const Wall LeverHandleParts[] = {
            { { 0.0f, 0.13f, 0.0f }, { 0.05f, 0.30f, 0.05f }, { 0.59f, 0.60f, 0.58f } },
            { { 0.0f, 0.28f, 0.0f }, { 0.20f, 0.08f, 0.08f }, { 0.76f, 0.32f, 0.17f } }
        };

        bool UploadBoxSurface(Mesh& mesh, std::span<const Wall> boxes)
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            BuildBoxSurface(boxes, vertices, indices);
            if (indices.empty())
                return false;

            mesh.Upload(vertices, indices);
            return mesh.IsValid();
        }
    }

    bool Renderer::Initialize(int width, int height)
    {
        SetViewport(width, height);
        glDepthFunc(GL_LESS);
        glClearDepth(1.0);
        return m_sceneShader.LoadFromFiles("shaders/scene.vert", "shaders/scene.frag")
            && m_cube.CreateCube(1.0f, 1.0f, 1.0f)
            && UploadBoxSurface(m_batteryMesh, BatteryParts)
            && UploadBoxSurface(m_heldLanternMesh, HeldLanternParts)
            && UploadBoxSurface(m_leverHousingMesh, LeverHousingParts)
            && UploadBoxSurface(m_leverHandleMesh, LeverHandleParts);
    }

    void Renderer::Shutdown()
    {
        m_levelMesh.Destroy();
        m_batteryMesh.Destroy();
        m_leverHousingMesh.Destroy();
        m_leverHandleMesh.Destroy();
        m_heldLanternMesh.Destroy();
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

    bool Renderer::PrepareLevel(const Level& level)
    {
        std::vector<Wall> boxes;
        boxes.reserve(level.Walls().size() + level.Fixtures().size() + level.Surfaces().size());
        boxes.insert(boxes.end(), level.Walls().begin(), level.Walls().end());
        boxes.insert(boxes.end(), level.Fixtures().begin(), level.Fixtures().end());
        boxes.insert(boxes.end(), level.Surfaces().begin(), level.Surfaces().end());

        return UploadBoxSurface(m_levelMesh, boxes);
    }

    void Renderer::DrawLevel()
    {
        DrawMesh(m_levelMesh, glm::mat4(1.0f), glm::vec3(1.0f));
    }

    void Renderer::DrawBatteryPickup(const BatteryPickup& battery)
    {
        if (!battery.available)
            return;

        DrawMesh(m_batteryMesh, glm::translate(glm::mat4(1.0f), battery.position), glm::vec3(1.0f));
    }

    void Renderer::DrawLever(const Lever& lever)
    {
        const glm::mat4 base = glm::translate(glm::mat4(1.0f), lever.position);
        DrawMesh(m_leverHousingMesh, base, glm::vec3(1.0f));

        // 손잡이의 두 자세 모두 방 안쪽(-Z)을 향한다.
        const float angle = glm::radians(lever.raised ? -30.0f : -145.0f);
        const glm::mat4 pivot = glm::translate(base, { 0.0f, 0.0f, -0.035f });
        DrawMesh(m_leverHandleMesh, glm::rotate(pivot, angle, { 1.0f, 0.0f, 0.0f }), glm::vec3(1.0f));

        const glm::mat4 indicator = glm::scale(glm::translate(base, { 0.125f, 0.185f, 0.0f }), { 0.07f, 0.07f, 0.04f });
        const glm::vec3 color = lever.raised ? glm::vec3(0.30f, 0.70f, 0.52f) : glm::vec3(0.76f, 0.39f, 0.20f);
        DrawMesh(m_cube, indicator, color);
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

        // BeginScene()에서 설정한 투영 행렬은 그래도 사용한다.
        m_sceneShader.SetMat4("view", glm::mat4(1.0f));

        DrawMesh(m_heldLanternMesh, glm::mat4(1.0f), glm::vec3(intensity));
    }

    void Renderer::EndScene()
    {
        glDisable(GL_DEPTH_TEST);
    }
}
