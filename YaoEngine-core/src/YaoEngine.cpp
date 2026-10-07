#include<glad/glad.h>
#include"./YaoEngine.h"
#include<cassert>
#include"./core/Windows/GLFWWindow.h"
#include<Event/Input.h>
#include<Event/WindowEvent.h>
#include"core/YaoMath/Math.h"
#include"core/Time/Time.h"
#include"core/ScriptEngine/ScriptEngine.h"
#include<Header.cuh>
#include<tracy/Tracy.hpp>
namespace YaoEngine {
	YaoEngine* YaoEngine::m_instance = nullptr;

	YaoEngine::YaoEngine() {
		ZoneScoped;
		assert(!m_instance);
		m_instance = this;
		m_window = YaoWindow::CreateWindowe();
		(static_cast<GLFWWindow*>(m_window.get()))->SetEventCallback([this](Event& e) { this->OnEvent(e); });
		m_Entitylayerstack = CreateScope<Layerstack>();
		m_UIlayerstack = CreateScope<Layerstack>();
		Input::Init();
	}

	YaoEngine::~YaoEngine() {
		if (m_UIlayerstack->Get().size() > 0) {
			for (Layer* layer : m_UIlayerstack->Get()) {
				if(layer)
				delete layer;
			}
		}
		if (m_Entitylayerstack->Get().size() > 0) {
			for (Layer* layer : m_Entitylayerstack->Get()) {
				if(layer)
				delete layer;
			}
		}
	}
	void YaoEngine::OnEvent(Event& e) {
		ZoneScoped;
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(
			[this](Event& e) { return this->WindowClose(e); });
		dispatcher.Dispatch<WindowResizeEvent>(
			[this](Event& e) { return this->WindowResize(e); });
#ifndef YAO_UI_DISABLESHOW
		for (Layer* layer : m_UIlayerstack->Get()) {
			layer->OnEvent(e);
		}
#endif
		for (Layer* layer : m_Entitylayerstack->Get()) {
			layer->OnEvent(e);
		}
	}
	void YaoEngine::run() {
		ZoneScopedN("YaoEngine::run");
		while (m_running)
		{
			m_window->Clear();
			m_window->OnUpdate();
			Time::Update();
			Input::OnUpdate();
			float Timestep = Time::DeltaTime();
#ifndef YAO_UI_DISABLESHOW
			for (Layer* layer : m_UIlayerstack->Get()) {
				layer->OnUpdate(Timestep);
			}
#endif
			for (Layer* layer : m_Entitylayerstack->Get()) {
				layer->OnUpdate(Timestep);
			}
		}
	}
	bool YaoEngine::WindowClose(Event& e) {
		m_running = false;
		return true;
	}
	bool YaoEngine::WindowResize(Event& e) {
		WindowResizeEvent& event = (WindowResizeEvent&)e;
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return false;
	}
	
}

