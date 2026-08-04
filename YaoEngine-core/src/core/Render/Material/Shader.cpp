#include<glad/glad.h>
#include<sstream>
#include<fstream>
#include<glm/gtc/type_ptr.hpp>
#include"Shader.h"
#include"../../Log/Log.h"
namespace YaoEngine {
	Shader::Shader(std::filesystem::path filepath)
	{
		ShaderSource source;
		Utill::ReadShaderFile(source, filepath);
		unsigned int vertexshader = glCreateShader(GL_VERTEX_SHADER);
		const char* vertexsource = source.vertexsource.c_str();
		glShaderSource(vertexshader, 1, &vertexsource, nullptr);
		glCompileShader(vertexshader);
		GLint success;
		

		glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(vertexshader, 1024, nullptr, infoLog);

			Yaoerror("Vertex Shader Compile Error:\n%s", infoLog);
		}
		unsigned int fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragmentsource = source.fragmentsource.c_str();
		glShaderSource(fragmentshader, 1, &fragmentsource, nullptr);
		glCompileShader(fragmentshader);

		glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(fragmentshader, 1024, nullptr, infoLog);

			Yaoerror("Fragment Shader Compile Error:\n%s", infoLog);
		}

		m_rendererID = glCreateProgram();
		glAttachShader(m_rendererID, vertexshader);
		glAttachShader(m_rendererID, fragmentshader);
		glLinkProgram(m_rendererID);
		glGetProgramiv(m_rendererID, GL_LINK_STATUS, &success);

		if (!success)
		{
			char infoLog[1024];
			glGetProgramInfoLog(m_rendererID, 1024, nullptr, infoLog);

			Yaoerror("Shader Link Error: %s", infoLog);
		}
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
			std::cout << std::filesystem::current_path() << std::endl;
			if (!stream.is_open())
			{
				Yaoerror("Failed to open shader file: %s", filepath.string().c_str());
				return;
			}
			std::string line;
			std::stringstream ss[2];
			while (std::getline(stream, line))
			{
				if (line.find("#vertexShader") != std::string::npos)
				{
					while (std::getline(stream, line) && line.find("#end_vertexShader") == std::string::npos)
					{
						ss[0] << line << "\n";
					}
				}
				if (line.find("#fragmentShader") != std::string::npos)
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
	void Shader::SetUniformMat4f(const std::string& name, Mat<4, 4, float> value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_rendererID, name.c_str()), 1, GL_TRUE, value.GetData().data());
	}
	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_rendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetUniformMat4f(const std::string& name, const float* value, bool transpose)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_rendererID, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, value);
	}
	void Shader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_rendererID, name.c_str()), value);
	}
	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		glUniform4f(glGetUniformLocation(m_rendererID, name.c_str()), v0, v1, v2, v3);
	}
	void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
	{
		glUniform3f(glGetUniformLocation(m_rendererID, name.c_str()), v0, v1, v2);
	}
	void Shader::SetUniform2f(const std::string& name, float v0, float v1)
	{
		glUniform2f(glGetUniformLocation(m_rendererID, name.c_str()), v0, v1);
	}
	void Shader::SetUniform1f(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_rendererID, name.c_str()), value);
	}
	void Shader::SetUniform1iv(const std::string& name, int count, const int* value)
	{
		glUniform1iv(glGetUniformLocation(m_rendererID, name.c_str()), count, value);
	}
	void Shader::SetUniform1fv(const std::string& name, int count, const float* value)
	{
		glUniform1fv(glGetUniformLocation(m_rendererID, name.c_str()), count, value);
	}
}
