#include<glad/glad.h>
#include"./GLFWwindow.h"
#include<Event/MouseEvent.h>
#include<Event/KeybroadEvent.h>
#include<Event/WindowEvent.h>
#include"../Log/Log.h"
namespace YaoEngine
{
	Scope<YaoWindow> YaoWindow::CreateWindowe(const Windowprop& prop)
	{
#ifdef GLFW_Window
		return Scope<GLFWWindow>(new GLFWWindow(prop));
#endif
	}

	GLFWWindow::GLFWWindow(const Windowprop&prop)
	{
		m_windowdata.height = prop.height;
		m_windowdata.width = prop.width;
		m_windowdata.title = prop.title;
		if(!glfwInit())
		{
			Yaoerror("Failed to initialize GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_window = glfwCreateWindow(m_windowdata.width, m_windowdata.height, m_windowdata.title, nullptr, nullptr);
		YaoInfo("GLFW window created width=%d  hegiht= %d ", m_windowdata.width, m_windowdata.height);
		Yaoerror("GLFW window created width=%d  hegiht= %d ", m_windowdata.width, m_windowdata.height);
		glfwSetWindowUserPointer(m_window, &m_windowdata);
		glfwMakeContextCurrent(m_window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Yaoerror("Failed to load GLAD");
		}

		glfwSetWindowCloseCallback(m_window,
			[](GLFWwindow* window)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				if (data->eventcallback)
				{
					WindowCloseEvent event;
					data->eventcallback(event);
				}
			});
		glfwSetWindowPosCallback(m_window,
			[](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				if (data->eventcallback)
				{
					WindowMoveEvent event(xpos, ypos);
					data->eventcallback(event);
				}
			});

		glfwSetWindowSizeCallback(m_window,
			[](GLFWwindow* window, int width, int height)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				if (data->eventcallback)
				{
					WindowResizeEvent event(width, height);
					data->eventcallback(event);
				}
			});
		glfwSetCursorPosCallback(m_window,
			[](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				if (data->eventcallback)
				{
					MouseMovedEvent event(xpos, ypos);
					data->eventcallback(event);
				}
			});
		glfwSetMouseButtonCallback(m_window,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				if (data->eventcallback)
				{
					switch (action)
					{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(static_cast<MouseButton>(button));
						data->eventcallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(static_cast<MouseButton>(button));
						data->eventcallback(event);
						break;
					}
					default:
						break;
					}
				}
			});

	}
}