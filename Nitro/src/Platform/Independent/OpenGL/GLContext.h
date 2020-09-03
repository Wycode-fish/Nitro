#pragma once

#include "Nitro/Render/IRenderingContext.h"

// @ Forward declaration of gl native window.
struct GLFWwindow;

namespace Nitro
{
	namespace Graphics
	{
		class GLContext : public IRenderingContext
		{
		public:
			GLContext(GLFWwindow* glfwWindow);
			virtual void Init() override;
			virtual void SwapBuffers() override;
		public:
			inline GLFWwindow* GetWindowHandle() const { return m_HWnd; }

		protected:
			virtual void LogCtxSpecs() override;

		protected:
			GLFWwindow* m_HWnd;
		};
	}
}