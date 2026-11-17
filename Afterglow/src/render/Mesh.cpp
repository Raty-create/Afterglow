#include "render/Mesh.h"

namespace ag
{
    Mesh::~Mesh()
    {
        Destroy();
    }

    void Mesh::Upload(const std::vector<GLfloat>& verts, const std::vector<GLuint>& indices)
    {
        Destroy();

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        m_vertexCount = static_cast<GLuint>(verts.size() / 9);
        m_indexCount  = (GLuint)indices.size();
    }

    // 육면체 지오메트리 생성 (원점 중심, 면마다 법선 1개)
    void Mesh::MakeBoxVerts(float w, float h, float d, const glm::vec3& color, std::vector<GLfloat>& verts, std::vector<GLuint>& indices)
    {
        const float hx = w * 0.5f, hy = h * 0.5f, hz = d * 0.5f;

        const glm::vec3 Face[6][4] =
        {
            { { -1,-1, 1 },{ 1,-1, 1 },{ 1, 1, 1 },{ -1, 1, 1 } },		// +z
            { { 1,-1,-1 },{ -1,-1,-1 },{ -1, 1,-1 },{ 1, 1,-1 } },		// -z
            { { 1,-1, 1 },{ 1,-1,-1 },{ 1, 1,-1 },{ 1, 1, 1 } },		// +x
            { { -1,-1,-1 },{ -1,-1, 1 },{ -1, 1, 1 },{ -1, 1,-1 } },	// -x
            { { -1, 1, 1 },{ 1, 1, 1 },{ 1, 1,-1 },{ -1, 1,-1 } },		// +y
            { { -1,-1,-1 },{ 1,-1,-1 },{ 1,-1, 1 },{ -1,-1, 1 } }		// -y
        };

        // Face[] 순서와 같은 면 법선
        const glm::vec3 FaceNormal[6] =
        {
            { 0,  0,  1 },	// +z
            { 0,  0, -1 },	// -z
            { 1,  0,  0 },	// +x
            { -1,  0,  0 },	// -x
            { 0,  1,  0 },	// +y
            { 0, -1,  0 }	// -y
        };

        verts.clear(); indices.clear();
        for (int f = 0; f < 6; ++f)
        {
            for (int v = 0; v < 4; ++v)
            {
                // 위치
                verts.push_back(Face[f][v].x * hx);
                verts.push_back(Face[f][v].y * hy);
                verts.push_back(Face[f][v].z * hz);
                // 색상
                verts.push_back(color.r);
                verts.push_back(color.g);
                verts.push_back(color.b);
                // 노멀
                verts.push_back(FaceNormal[f].x);
                verts.push_back(FaceNormal[f].y);
                verts.push_back(FaceNormal[f].z);
            }
            const GLuint b = static_cast<GLuint>(f * 4);
            indices.insert(indices.end(), { b, b + 1, b + 2,   b, b + 2, b + 3 });
        }
    }

    // 24개 정점(면마다 노멀 분리) + 36개 인덱스로 큐브를 만든다.
    bool Mesh::CreateCube(float width, float height, float depth)
    {
        if (width <= 0.0f || height <= 0.0f || depth <= 0.0f)
            return false;

        std::vector<GLfloat> verts;
        std::vector<GLuint> indices;

        MakeBoxVerts(width, height, depth, glm::vec3(0.25f), verts, indices);
        Upload(verts, indices);
        return IsValid();
    }

    // 구 지오메트리 생성 (원점 중심, 반지름 radius)
    // 구는 중심이 원점이므로 정규화된 위치가 곧 노멀
    void Mesh::MakeSphereVerts(float radius, int segments, const glm::vec3& color, std::vector<GLfloat>& verts, std::vector<GLuint>& indices)
    {
        const int SLICES = segments;	// 경선 방향 분할
        const int STACKS = segments / 2;	// 위선 방향 분할

        verts.clear();
        indices.clear();
        verts.reserve((SLICES + 1) * (STACKS + 1) * 9);
        indices.reserve(SLICES * STACKS * 6);

        // 위도(phi) 0~pi, 경도(theta) 0~2pi 로 구의 정점 배치
        for (int i = 0; i <= STACKS; ++i)
        {
            float phi = glm::pi<float>() * i / STACKS;

            for (int j = 0; j <= SLICES; ++j)
            {
                float theta = glm::two_pi<float>() * j / SLICES;

                float nx = std::sinf(phi) * std::cosf(theta);
                float ny = std::cosf(phi);
                float nz = std::sinf(phi) * std::sinf(theta);

                // 위치
                verts.push_back(radius * nx);
                verts.push_back(radius * ny);
                verts.push_back(radius * nz);
                // 색상
                verts.push_back(color.r);
                verts.push_back(color.g);
                verts.push_back(color.b);
                // 노멀 (단위 구 기준)
                verts.push_back(nx);
                verts.push_back(ny);
                verts.push_back(nz);
            }
        }

        // 인접한 4개 정점을 2개의 삼각형으로
        for (int i = 0; i < STACKS; ++i)
        {
            for (int j = 0; j < SLICES; ++j)
            {
                const GLuint a = static_cast<GLuint>(i * (SLICES + 1) + j);
                const GLuint b = a + SLICES + 1;

                if (i > 0)
                    indices.insert(indices.end(), { a, a + 1, b });

                if (i < STACKS - 1)
                    indices.insert(indices.end(), { a + 1, b + 1, b });
            }
        }
    }

    // 위도/경도 분할로 구 정점과 인덱스를 생성한다.
    bool Mesh::CreateSphere(float radius, int segments)
    {
        if (radius <= 0.0f || segments < 4)
            return false;

        std::vector<GLfloat> verts;
        std::vector<GLuint> indices;

        MakeSphereVerts(radius, segments, glm::vec3(1.0f), verts, indices);
        Upload(verts, indices);
        return IsValid();
    }

    void Mesh::Destroy()
    {
        if (m_vao != 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            glDeleteBuffers(1, &m_vbo);
            glDeleteBuffers(1, &m_ebo);
            m_vao = m_vbo = m_ebo = 0;
            m_vertexCount = m_indexCount = 0;
        }
    }

    void Mesh::Draw() const
    {
        if (!IsValid() || m_indexCount == 0)
            return;

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}
