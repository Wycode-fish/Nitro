#include "NtPCH.h"
#include "OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Nitro
{
	namespace Graphics
	{
		OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
			: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f))
			, m_ViewMatrix(1.0f)
			, m_Position(0.0f)
		{
			this->m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}
		void OrthographicCamera::ReCalculateViewMatrix()
		{
			// @ by setting the rotate axis to vec3(0.0f, 0.0f, 1.0f/*instead of -1*/);, we're rotating 
			// @ counter clock-wise.
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			this->m_ViewMatrix = glm::inverse(transform);
			this->m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}
	}
}