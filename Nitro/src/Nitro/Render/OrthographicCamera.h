#pragma once

#include <glm/glm.hpp>

namespace Nitro
{
	namespace Graphics
	{
		// @ only used in 2D rendering
		class OrthographicCamera
		{
		public:
			OrthographicCamera(float left, float right, float bottom, float top);
			void ReCalculateViewMatrix();

			inline void SetPosition(const glm::vec3& pos) { m_Position = pos; this->ReCalculateViewMatrix(); }
			inline void SetRotation(const float rotation) { m_Rotation = rotation; this->ReCalculateViewMatrix(); }
			inline void SetProjectionMatrix(const glm::mat4& pm) { m_ProjectionMatrix = pm; }
			inline void SetViewMatrix(const glm::mat4& vm) { m_ViewMatrix = vm; }

			inline const float		GetRotation() const { return m_Rotation; }
			inline const glm::vec3	GetPosition() const { return m_Position; }
			inline const glm::mat4	GetProjectionMatrix() const { return m_ProjectionMatrix; }
			inline const glm::mat4	GetViewMatrix() const { return m_ViewMatrix; }
			inline const glm::mat4  GetVPMatrix() const { return m_VPMatrix; }

		private:
			glm::mat4 m_ProjectionMatrix;
			glm::mat4 m_ViewMatrix;
			glm::mat4 m_VPMatrix;
			// @ --- probably use transform later
			glm::vec3 m_Position;
			float m_Rotation = 0.0f;
		};
	}
}