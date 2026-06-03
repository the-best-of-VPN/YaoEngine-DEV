#pragma once
#include<filesystem>
namespace YaoEngine
{
	class Shader {
	public:
		Shader(std::filesystem::path);//将顶点和片段着色器放在同一个文件中，使用#shader vertex和#shader fragment区分
		~Shader();

		void Bind() const;
		void UnBind() const;

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