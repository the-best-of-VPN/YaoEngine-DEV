#include<glad/glad.h>
#include<sstream>
#include<fstream>
#include"Shader.h"
namespace YaoEngine {
	Shader::Shader(std::filesystem::path filepath)
	{
		ShaderSource source;
		Utill::ReadShaderFile(source, filepath);
		unsigned int vertexshader = glCreateShader(GL_VERTEX_SHADER);
		const char* vertexsource = source.vertexsource.c_str();
		glShaderSource(vertexshader, 1, &vertexsource, nullptr);
		glCompileShader(vertexshader);
		unsigned int fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragmentsource = source.fragmentsource.c_str();
		glShaderSource(fragmentshader, 1, &fragmentsource, nullptr);
		glCompileShader(fragmentshader);
		m_rendererID = glCreateProgram();
		glAttachShader(m_rendererID, vertexshader);
		glAttachShader(m_rendererID, fragmentshader);
		glLinkProgram(m_rendererID);
		glDeleteShader(vertexshader);
		glDeleteShader(fragmentshader);
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_rendererID);
	}
	void Shader::Bind() const
	{
		glUseProgram(m_rendererID);
	}
	void Shader::UnBind() const
	{
		glUseProgram(0);
	}
	namespace Utill {
		void ReadShaderFile(ShaderSource& source, const std::filesystem::path& filepath)
		{
			std::ifstream stream(filepath);
			std::string line;
			std::stringstream ss[2];
			while (std::getline(stream, line))
			{
				if (line.find("#vertexShader ") != std::string::npos)
				{
					while (std::getline(stream, line) && line.find("#end_vertexShader") == std::string::npos)
					{
						ss[0] << line << "\n";
					}
				}
				if (line.find("#fragmentShader ") != std::string::npos)
				{
					while (std::getline(stream, line) && line.find("#end_fragmentShader") == std::string::npos)
					{
						ss[1] << line << "\n";
					}
				}
			}
			source.vertexsource = ss[0].str();
			source.fragmentsource = ss[1].str();
		}
	}
}