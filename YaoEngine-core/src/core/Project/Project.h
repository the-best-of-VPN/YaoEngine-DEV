#pragma once
#include<string>
#include<filesystem>
namespace YaoEngine {
	struct ProjectConfig {
		std::string name;

	};
	class Project {
	public:

	private:
		ProjectConfig m_ProjectConfig;
		std::filesystem::path m_currentDirectory;
	};
}