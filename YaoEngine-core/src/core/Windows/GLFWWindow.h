#pragma once
#include"./windows.h"
#include<GLFW/glfw3.h>
#include<functional>
#include<Event/Event.h>
#include<tracy/Tracy.hpp>
namespace YaoEngine
{
	class GLFWWindow : public YaoWindow
	{
	public:
		GLFWWindow(const Windowprop&);
		virtual ~GLFWWindow() override { glfwDestroyWindow(m_window);glfwTerminate(); };
		virtual void* GetNativeWindow()override { return m_window; };
		virtual void OnUpdate() override { 
			glfwPollEvents();
			glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
		
		};
		virtual void Clear() override {  
			ZoneScopedN("Clear");
		glClearColor(0.7f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		};

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_windowdata.eventcallback = callback; }
	private:
		GLFWwindow* m_window;
		struct WindowData {
			unsigned int width, height;
			const char* title;
			std::function<void(Event&)> eventcallback;
			WindowData(unsigned int width = 1000, unsigned int height = 800, const char* title = "YaoEngine") : width(width), height(height), title(title) {}
		}m_windowdata;
	};
}