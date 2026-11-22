#include "platform/Window.h"

#include "platform/Input.h"

namespace ag
{
	bool Window::Create(const char* title, int width, int height, Input* input)
	{
		// GLFW 초기화
		if (!glfwInit())
		{
			std::cerr << "Failed to Initialize GLFW\n";
			return false;
		}

		// OpenGL 버전 설정
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);

		// 기본 모니터 가져오기
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		// 모니터의 현재 비디오 모드(해상도 및 주사율) 정보 얻기
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// 창 생성
		m_handle = glfwCreateWindow(width, height, title, monitor, nullptr);
		if (!m_handle)
		{
			std::cerr << "Failed to Create Window\n";
			glfwTerminate();
			return false;
		}

		// OpenGL 컨텍스트 연결
		glfwMakeContextCurrent(m_handle);

		// GLEW 초기화
		const GLenum result = glewInit();
		if (result != GLEW_OK)
		{
			std::cerr << "Failed to Initialize GLEW : " << glewGetErrorString(result) << '\n';
			Destroy();
			return false;
		}

		m_input = input;
		glfwGetFramebufferSize(m_handle, &m_width, &m_height);

		glfwSetWindowUserPointer(m_handle, this);
		glfwSetWindowFocusCallback(m_handle, [](GLFWwindow* handle, int)
			{
				auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				window->m_hasCursorSample = false;
			});
		glfwSetKeyCallback(m_handle, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
			{
				auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				if (!window->m_input)
					return;

				if (action == GLFW_PRESS)
					window->m_input->OnKeyDown(key);
				else if (action == GLFW_RELEASE)
					window->m_input->OnKeyUp(key);
			});
		glfwSetScrollCallback(m_handle, [](GLFWwindow* handle, double xoffset, double yoffset)
			{
				auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				if (window->m_input)
					window->m_input->OnMouseWheel(static_cast<float>(yoffset));
			});

		return true;
	}

	void Window::Destroy()
	{
		if (!m_handle)
			return;

		glfwDestroyWindow(m_handle);
		m_handle = nullptr;
		m_input = nullptr;
		glfwTerminate();
	}

	bool Window::ShouldClose() const
	{
		return !m_handle || glfwWindowShouldClose(m_handle);
	}

	void Window::CaptureCursor()
	{
		glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_hasCursorSample = false;
	}

	bool Window::PumpMessages()
	{
		glfwPollEvents();
		if (ShouldClose())
			return false;

		glfwGetFramebufferSize(m_handle, &m_width, &m_height);
		if (m_input && glfwGetWindowAttrib(m_handle, GLFW_FOCUSED)
			&& glfwGetInputMode(m_handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			double x, y;
			glfwGetCursorPos(m_handle, &x, &y);
			if (m_hasCursorSample)
				m_input->OnMouseMove(static_cast<float>(x - m_cursorX), static_cast<float>(y - m_cursorY));
			m_cursorX = x;
			m_cursorY = y;
			m_hasCursorSample = true;
		}
		return true;
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_handle);
	}
}
