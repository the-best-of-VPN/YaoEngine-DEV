#pragma once
#include<filesystem>
#include<string>
#include<glm/glm.hpp>
#define  Yao_Game_Matrix
#include"../../YaoMath/Math.h"
namespace YaoEngine
{
	class Shader {
	public:
		Shader(std::filesystem::path);//将顶点和片段着色器放在同一个文件中，使用#shader vertex和#shader fragment区分
		~Shader();

		void Bind() const;
		void UnBind() const;

		void SetUniformMat4f(const std::string& name,Mat<4,4,float>  value);
		void SetUniformMat4f(const std::string& name, const glm::mat4& value);
		void SetUniformMat4f(const std::string& name, const float* value, bool transpose = false);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform2f(const std::string& name, float v0, float v1);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1iv(const std::string& name, int count, const int* value);
		void SetUniform1fv(const std::string& name, int count, const float* value);



		unsigned int GetID() const { return m_rendererID; }
	private:
		unsigned int m_rendererID;
	};
	struct ShaderSource
	{
		std::string vertexsource;
		std::string fragmentsource;
	};
	namespace Utill {
		void ReadShaderFile(ShaderSource &source, const std::filesystem::path &filepath);
	}
}
