#include"./YaoEngine.h"
#include<functional>
#include<assert.h>
#include"./core/Windows/GLFWWindow.h"
#include<Event/MouseEvent.h>
#include<Event/KeybroadEvent.h>
#include<Event/WindowEvent.h>
#include"core/YaoMath/Math.h"
#include"core/Time/Time.h"
#include"core/ScriptEngine/ScriptEngine.h"
namespace YaoEngine {
	YaoEngine* YaoEngine::m_instance = nullptr;

	YaoEngine::YaoEngine() {
		assert(!m_instance);
		m_instance = this;
		m_window = YaoWindow::CreateWindowe();
		(static_cast<GLFWWindow*>(m_window.get()))->SetEventCallback([this](Event& e) { this->OnEvent(e); });
		m_Entitylayerstack = CreateScope<Layerstack>();
		m_UIlayerstack = CreateScope<Layerstack>();

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
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(
			[this](Event&e) { return this->WindowClose(); });
		dispatcher.Dispatch<WindowResizeEvent>(
			[this](Event& e){ return this->WindowResize(); });
#ifndef YAO_UI_SHOW
		if (m_UIlayerstack->Get().size() > 0) {
			for (Layer* layer : m_UIlayerstack->Get()) {
				layer->OnEvent(e);
			}
		}
#endif
		if (m_Entitylayerstack->Get().size() > 0) {
			for (Layer* layer : m_Entitylayerstack->Get()) {
				layer->OnEvent(e);
			}
		}
	}

	void YaoEngine::run() {
		while (m_running)
		{
			m_window->Clear();
			Time::Update();
#ifndef YAO_UI_SHOW
			if (m_UIlayerstack->Get().size() > 0) {
			for (Layer* layer : m_UIlayerstack->Get()) {
				layer->OnUpdate(Time::DeltaTime());
			}
		}
#endif
			if (m_Entitylayerstack->Get().size() > 0) {
				for (Layer* layer : m_Entitylayerstack->Get()) {
					layer->OnUpdate(Time::DeltaTime());
				}
			}
			m_window->OnUpdate();
		}
	}
}

