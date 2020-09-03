#include "NtPCH.h"
#include "GLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Nitro
{
	namespace Graphics
	{
		GLContext::GLContext(GLFWwindow * glfwWindow)
			: m_HWnd{glfwWindow}
		{
			NT_CORE_ASSERT(m_HWnd, "window handle nullptr.");
		}
		void GLContext::Init()
		{
			glfwMakeContextCurrent(m_HWnd);
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			NT_CORE_ASSERT(status, "glad initialization failed.");

			this->LogCtxSpecs();
		}
		void GLContext::SwapBuffers()
		{
			glfwSwapBuffers(m_HWnd);
		}

		void GLContext::LogCtxSpecs()
		{
			NT_CORE_INFO("========== OpenGL specifications ==========");
			NT_CORE_INFO("vendor:	{0}", glGetString(GL_VENDOR));
			NT_CORE_INFO("gpu:		{0}", glGetString(GL_RENDERER));
			NT_CORE_INFO("version:	{0}", glGetString(GL_VERSION));
			NT_CORE_INFO("========== ===================== ==========");
		}
	}
}