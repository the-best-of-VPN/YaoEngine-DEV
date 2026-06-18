#pragma once
#define Yao_Game_Matrix
#include<core.h>
#include "../../YaoMath/Math.h"
namespace YaoEngine {
	class Camera {
	public:
		enum class ProjectionType
		{
			None=-1,Perspective = 0, Orthographic = 1
		};
		Camera(Mat<4, 1, float> position, Mat<4, 1, float> target, Mat<4, 1, float> up)
		{
				auto forward = Normalize(target - position);
				auto right = Normalize(Cross(forward, up));
				auto realUp = Cross(right, forward);

				m_view = Mat<4, 4, float>{
					right[0],    right[1],    right[2],    -Dot(right, position),
					realUp[0],   realUp[1],   realUp[2],   -Dot(realUp, position),
				   -forward[0], -forward[1], -forward[2],   Dot(forward, position),
					0.0f,        0.0f,        0.0f,         1.0f
				};

				m_projectionType = ProjectionType::None;
		}
		void SetPerspective(float fov, float aspectRatio, float near, float far) {
			m_projectionType = ProjectionType::Perspective;
			float tanHalfFov = tanf(fov / 2.0f);
			m_data = Mat<4, 4, float>{
				1.0f / (aspectRatio * tanHalfFov),0.0f,0.0f,0.0f,
				0.0f,1.0f / tanHalfFov,0.0f,0.0f,
				0.0f,0.0f,-(far + near) / (far - near),-(2.0f * far * near) / (far - near),
				0.0f,0.0f,-1.0f,0.0f
			};
		}
		void SetOrthographic(float left, float right, float bottom, float top, float near, float far) {
			m_projectionType = ProjectionType::Orthographic;
			m_data = Mat<4, 4, float>{
				2.0f / (right - left),0.0f,0.0f,-(right + left) / (right - left),
				0.0f,2.0f / (top - bottom),0.0f,-(top + bottom) / (top - bottom),
				0.0f,0.0f,-2.0f / (far - near),-(far + near) / (far - near),
				0.0f,0.0f,0.0f,1.0f
			};
		}
	private:
		Mat<4, 4, float> m_view = Mat<4, 4, float>{};
		Mat<4, 4, float> m_data = Mat<4,4,float>{};
		ProjectionType m_projectionType = ProjectionType::None;
	};
}