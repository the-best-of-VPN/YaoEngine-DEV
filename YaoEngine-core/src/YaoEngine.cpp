#include"./YaoEngine.h"
#include<cassert>
#include"./core/Windows/GLFWWindow.h"
#include<Event/WindowEvent.h>
#include"core/YaoMath/Math.h"
#include"core/Time/Time.h"
#include"core/ScriptEngine/ScriptEngine.h"
#include<Header.cuh>
namespace YaoEngine {
	YaoEngine* YaoEngine::m_instance = nullptr;

	YaoEngine::YaoEngine() {
		assert(!m_instance);
		m_instance = this;
		m_window = YaoWindow::CreateWindowe();
		(static_cast<GLFWWindow*>(m_window.get()))->SetEventCallback([this](Event& e) { this->OnEvent(e); });
		m_Entitylayerstack = CreateScope<Layerstack>();
		m_UIlayerstack = CreateScope<Layerstack>();
		m_quad = Mesh::Create(
			std::vector<float>{
				-0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 1.0f, 1.0f
			},
			std::vector<unsigned int>{
				0,1,2,
				2,3,0
			},
			BufferLayout{
				{ShaderDataType::Float3,"a_pos"},
			  //{ShaderDataType::Float2,"aTexCoord"},
				{ShaderDataType::Float2,"aTexCoord"},
			}
		);
		m_material = Material2D::Create("../../../../YaoEngine-core/Shader.yao/Shader.yao", "../../../../YaoEngine-core/Asset/huajiao.jpg");
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
			[this](Event&e) { return this->WindowClose(e); });
		dispatcher.Dispatch<WindowResizeEvent>(
			[this](Event& e){ return this->WindowResize(e); });
#ifndef YAO_UI_DISABLESHOW
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
			float Timestep = Time::DeltaTime();
#ifndef YAO_UI_DISABLESHOW
			if (m_UIlayerstack->Get().size() > 0) {
			for (Layer* layer : m_UIlayerstack->Get()) {
				layer->OnUpdate(Timestep);
			}
		}
#endif
			if (m_Entitylayerstack->Get().size() > 0) {
				for (Layer* layer : m_Entitylayerstack->Get()) {
					layer->OnUpdate(Timestep);
				}
			}
			Renderer2D::Draw(m_quad, m_material);
			m_window->OnUpdate();
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

