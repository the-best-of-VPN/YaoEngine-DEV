#pragma once
#include<core.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
namespace YaoEngine {
	class Camera {
	public:
		enum class ProjectionType
		{
			None=-1,Perspective = 0, Orthographic = 1
		};
		Camera();
		Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

		static Camera CreatePerspective(float verticalFovDegrees, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);
		static Camera CreateOrthographic(float left, float right, float bottom, float top, float nearClip = -1.0f, float farClip = 1.0f);

		void SetPerspective(float verticalFovDegrees, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);
		void SetOrthographic(float left, float right, float bottom, float top, float nearClip = -1.0f, float farClip = 1.0f);
		void SetViewportSize(float width, float height);

		void SetPosition(const glm::vec3& position);
		void Move(const glm::vec3& delta);
		void MoveLocal(const glm::vec3& delta);

		void SetRotation(const glm::vec3& rotationRadians);
		void SetRotationDegrees(const glm::vec3& rotationDegrees);
		void Rotate(const glm::vec3& deltaRadians);
		void RotateDegrees(const glm::vec3& deltaDegrees);
		void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

		const glm::mat4& GetProjectionMatrix() const { return m_projection; }
		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_viewProjection; }
		const float* GetViewProjectionData() const;

		const glm::vec3& GetPosition() const { return m_position; }
		glm::vec3 GetRotation() const;
		glm::vec3 GetRotationDegrees() const;
		const glm::vec3& GetForward() const { return m_forward; }
		const glm::vec3& GetRight() const { return m_right; }
		const glm::vec3& GetUp() const { return m_up; }
		ProjectionType GetProjectionType() const { return m_projectionType; }
	private:
		void RecalculateProjection();
		void RecalculateView();

		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_viewProjection = glm::mat4(1.0f);

		glm::vec3 m_position = glm::vec3(0.0f);
		glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

		float m_perspectiveFovDegrees = 45.0f;
		float m_perspectiveAspectRatio = 1.0f;
		float m_perspectiveNear = 0.1f;
		float m_perspectiveFar = 1000.0f;

		float m_orthographicLeft = -1.0f;
		float m_orthographicRight = 1.0f;
		float m_orthographicBottom = -1.0f;
		float m_orthographicTop = 1.0f;
		float m_orthographicNear = -1.0f;
		float m_orthographicFar = 1.0f;
		float m_orthographicSize = 2.0f;

		ProjectionType m_projectionType = ProjectionType::None;
	};
}
