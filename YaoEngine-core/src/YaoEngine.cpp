#include"./YaoEngine.h"
#include<GLFW/glfw3.h>
namespace YaoEngine {
	YaoEngine* YaoEngine::m_instance = nullptr;

	YaoEngine::YaoEngine() {
		m_instance = this;
		glfwInit();
		GLFWwindow* window = glfwCreateWindow(800, 600, "YaoEngine", NULL, NULL);
	}

	YaoEngine::~YaoEngine() {
	}


	void YaoEngine::run() {
		while (m_running)
		{
			
		}
	}
}

