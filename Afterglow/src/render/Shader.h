#pragma once

namespace ag
{
    // GLSL 셰이더 프로그램 하나를 감싼다. 파일 로드/컴파일/링크와 유니폼 설정을 담당한다.
    class Shader
    {
    public:
        ~Shader();

        bool LoadFromFiles(const char* vertexPath, const char* fragmentPath);
        void Destroy();

        void Use() const;
        bool IsValid() const { return m_shaderProgram != 0; }

        void SetInt(const char* name, int value) const;
        void SetFloat(const char* name, float value) const;
        void SetVec3(const char* name, const glm::vec3& value) const;
        void SetMat4(const char* name, const glm::mat4& value) const;

    private:
        bool Build(const char* vertexSource, const char* fragmentSource);
        GLuint Compile(unsigned int type, const char* source) const;

        GLuint m_shaderProgram = 0;
    };
}
