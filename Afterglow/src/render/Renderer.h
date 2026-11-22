#pragma once

#include "render/Shader.h"
#include "render/Mesh.h"

namespace ag
{
    class BatteryPickup;
    class Camera;
    class Lantern;
    class Mesh;
    class Level;
    class Player;

    // OpenGL 렌더링 진입점.
    // 씬 셰이더 하나로 불을 받는 모든 지오메트리(벽/바닥/괴물/소품)를 그린다.
    class Renderer
    {
    public:
        bool Initialize(int width, int height);
        void Shutdown();

        void SetViewport(int width, int height);

        // 뷰/투영 행렬과 랜턴(스포트라이트) 파라미터를 셰이더에 올린다.
        void BeginScene(const Camera& camera, const Lantern& lantern, const glm::vec3& lanternDirection);
        void BeginScene(const Camera& camera);
        void DrawLevel(const Level& level);
        void DrawBatteryPickup(const BatteryPickup& battery);
        void DrawPlayerBody(const Player& player, const Lantern& lantern);
        void DrawHeldLantern(const Lantern& lantern);

        void DrawMesh(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color);

        // HUD 같은 2D 요소를 그리기 전에 깊이 테스트를 끈다.
        void EndScene();

    private:
        Shader m_sceneShader;
        Mesh   m_cube;
        int    m_width  = 0;
        int    m_height = 0;
    };
}
