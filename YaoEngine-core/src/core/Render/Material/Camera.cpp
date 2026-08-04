#include "Camera.h"
#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace YaoEngine {

	namespace
	{
		constexpr float MinViewportSize = 1.0f;
		constexpr float MinAspectRatio = 0.0001f;
	}

	Camera::Camera()
	{
		SetOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	}

	Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
		: Camera()
	{
		m_position = position;
		LookAt(target, up);
	}

	Camera Camera::CreatePerspective(float verticalFovDegrees, float aspectRatio, float nearClip, float farClip)
	{
		Camera camera;
		camera.SetPerspective(verticalFovDegrees, aspectRatio, nearClip, farClip);
		return camera;
	}

	Camera Camera::CreateOrthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
	{
		Camera camera;
		camera.SetOrthographic(left, right, bottom, top, nearClip, farClip);
		return camera;
	}

	void Camera::SetPerspective(float verticalFovDegrees, float aspectRatio, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Perspective;
		m_perspectiveFovDegrees = verticalFovDegrees;
		m_perspectiveAspectRatio = std::max(aspectRatio, MinAspectRatio);
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;
		RecalculateProjection();
	}

	void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Orthographic;
		m_orthographicLeft = left;
		m_orthographicRight = right;
		m_orthographicBottom = bottom;
		m_orthographicTop = top;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;
		m_orthographicSize = std::max(top - bottom, MinViewportSize);
		RecalculateProjection();
	}

	void Camera::SetViewportSize(float width, float height)
	{
		width = std::max(width, MinViewportSize);
		height = std::max(height, MinViewportSize);

		const float aspectRatio = width / height;
		if (m_projectionType == ProjectionType::Perspective)
		{
			SetPerspective(m_perspectiveFovDegrees, aspectRatio, m_perspectiveNear, m_perspectiveFar);
			return;
		}

		const float halfHeight = m_orthographicSize * 0.5f;
		const float halfWidth = halfHeight * aspectRatio;
		SetOrthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, m_orthographicNear, m_orthographicFar);
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		m_position = position;
		RecalculateView();
	}

	void Camera::Move(const glm::vec3& delta)
	{
		m_position += delta;
		RecalculateView();
	}

	void Camera::MoveLocal(const glm::vec3& delta)
	{
		m_position += (m_right * delta.x) + (m_up * delta.y) + (m_forward * delta.z);
		RecalculateView();
	}

	void Camera::SetRotation(const glm::vec3& rotationRadians)
	{
		m_orientation = glm::normalize(glm::quat(rotationRadians));
		RecalculateView();
	}

	void Camera::SetRotationDegrees(const glm::vec3& rotationDegrees)
	{
		SetRotation(glm::radians(rotationDegrees));
	}

	void Camera::Rotate(const glm::vec3& deltaRadians)
	{
		m_orientation = glm::normalize(glm::quat(deltaRadians) * m_orientation);
		RecalculateView();
	}

	void Camera::RotateDegrees(const glm::vec3& deltaDegrees)
	{
		Rotate(glm::radians(deltaDegrees));
	}

	void Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
	{
		glm::vec3 direction = target - m_position;
		if (glm::length(direction) <= 0.000001f)
			return;

		direction = glm::normalize(direction);
		m_orientation = glm::normalize(glm::quatLookAt(direction, glm::normalize(up)));
		RecalculateView();
	}

	const float* Camera::GetViewProjectionData() const
	{
		return glm::value_ptr(m_viewProjection);
	}

	glm::vec3 Camera::GetRotation() const
	{
		return glm::eulerAngles(m_orientation);
	}

	glm::vec3 Camera::GetRotationDegrees() const
	{
		return glm::degrees(GetRotation());
	}

	void Camera::RecalculateProjection()
	{
		switch (m_projectionType)
		{
		case ProjectionType::Perspective:
			m_projection = glm::perspective(glm::radians(m_perspectiveFovDegrees), m_perspectiveAspectRatio, m_perspectiveNear, m_perspectiveFar);
			break;
		case ProjectionType::Orthographic:
			m_projection = glm::ortho(m_orthographicLeft, m_orthographicRight, m_orthographicBottom, m_orthographicTop, m_orthographicNear, m_orthographicFar);
			break;
		default:
			m_projection = glm::mat4(1.0f);
			break;
		}

		m_viewProjection = m_projection * m_view;
	}

	void Camera::RecalculateView()
	{
		m_forward = glm::normalize(m_orientation * glm::vec3(0.0f, 0.0f, -1.0f));
		m_right = glm::normalize(m_orientation * glm::vec3(1.0f, 0.0f, 0.0f));
		m_up = glm::normalize(m_orientation * glm::vec3(0.0f, 1.0f, 0.0f));
		m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
		m_viewProjection = m_projection * m_view;
	}
}
