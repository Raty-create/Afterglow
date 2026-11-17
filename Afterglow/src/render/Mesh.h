#pragma once

namespace ag
{
    // 정점 버퍼 하나를 소유하는 최소 메쉬.
    // 육면체/구를 기본 도형으로 쓰고, 일부 소품만 OBJ 로 대체한다.
    class Mesh
    {
    public:
        ~Mesh();

        void Upload(const std::vector<GLfloat>& verts, const std::vector<GLuint>& indices);
        void MakeBoxVerts(float w, float h, float d, const glm::vec3& color, std::vector<GLfloat>& verts, std::vector<GLuint>& indices);
        bool CreateCube(float width, float height, float depth);
        void MakeSphereVerts(float r, int segments, const glm::vec3& color, std::vector<GLfloat>& verts, std::vector<GLuint>& indices);
        bool CreateSphere(float radius = 0.5f, int segments = 16);
        void Destroy();

        void Draw() const;

        unsigned int VertexCount() const { return m_vertexCount; }
        bool IsValid() const { return m_vao != 0; }

    private:
        GLuint m_vao         = 0;
        GLuint m_vbo         = 0;
        GLuint m_ebo         = 0;
        GLuint m_vertexCount = 0;
        GLuint m_indexCount  = 0;
    };
}
