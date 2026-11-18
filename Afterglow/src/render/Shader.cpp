#include "render/Shader.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace ag
{
    Shader::~Shader()
    {
        Destroy();
    }

    bool Shader::LoadFromFiles(const char* vertexPath, const char* fragmentPath)
    {
        const auto ReadSource = [](const char* path, std::string& source)
            {
                std::ifstream file(path, std::ios::binary);
                if (!file)
                {
                    std::cerr << "Cannot open Shader File : " << path << '\n';
                    return false;
                }

                source.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

                if (source.empty())
                {
                    std::cerr << "Shader File is Empty : " << path << '\n';
                    return false;
                }

                return true;

            };

        std::string vertexSrc;
        std::string fragmentSrc;

        if (!ReadSource(vertexPath, vertexSrc) || !ReadSource(fragmentPath, fragmentSrc))
        {
            return false;
        }

        return Build(vertexSrc.c_str(), fragmentSrc.c_str());
    }

    void Shader::Destroy()
    {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }

    void Shader::Use() const
    {
        glUseProgram(m_shaderProgram);
    }

    void Shader::SetInt(const char* name, int value) const
    {
        const GLint location = glGetUniformLocation(m_shaderProgram, name);
        glUniform1i(location, value);
    }

    void Shader::SetFloat(const char* name, float value) const
    {
        const GLint location = glGetUniformLocation(m_shaderProgram, name);
        glUniform1f(location, value);
    }

    void Shader::SetVec3(const char* name, const glm::vec3& value) const
    {
        const GLint location = glGetUniformLocation(m_shaderProgram, name);
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const char* name, const glm::mat4& value) const
    {
        const GLint location = glGetUniformLocation(m_shaderProgram, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    bool Shader::Build(const char* vertexSource, const char* fragmentSource)
    {
        const GLuint vertex = Compile(GL_VERTEX_SHADER, vertexSource);
        if (vertex == 0)
        {
            return false;
        }

        const GLuint fragment = Compile(GL_FRAGMENT_SHADER, fragmentSource);
        if (fragment == 0)
        {
            glDeleteShader(vertex);
            return false;
        }

        const GLuint shaderProgram = glCreateProgram();
        if (shaderProgram == 0)
        {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        glAttachShader(shaderProgram, vertex);
        glAttachShader(shaderProgram, fragment);
        glLinkProgram(shaderProgram);
        glDetachShader(shaderProgram, vertex);
        glDetachShader(shaderProgram, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        GLint linked = GL_FALSE;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
        if (linked != GL_TRUE)
        {
            GLchar log[2048]{};
            glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
            std::cerr << "Failed to Link Shader\n" << log << '\n';
            glDeleteProgram(shaderProgram);
            return false;
        }

        Destroy();
        m_shaderProgram = shaderProgram;
        return true;
    }

    GLuint Shader::Compile(unsigned int type, const char* source) const
    {
        const char* stage = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";

        const GLuint shader = glCreateShader(type);
        if (shader == 0)
        {
            std::cerr << "Failed to Create Shader Object\n";
            return 0;
        }

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (compiled == GL_TRUE)
        {
            return shader;
        }

        GLchar log[2048]{};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << stage << "Failed to Compile Shader\n" << log << '\n';

        glDeleteShader(shader);
        return 0;
    }
}
