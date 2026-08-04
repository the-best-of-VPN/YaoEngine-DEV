#pragma once 
#include"./core/Windows/windows.h"
#include"core/Event/Event.h"
#include"core/Layer/Layerstack.h"
#include"core/Render/Material/Camera.h"
#include"core/Render/RenderAPI.h"

namespace YaoEngine {


	class YaoEngine {
	public:
		virtual ~YaoEngine();
		virtual void run();
		static YaoEngine* Getinstance() { return m_instance; }
		void OnEvent(Event&);
		//推送UI栈和渲染层
		inline void PushLayer(Layer* layer) { m_Entitylayerstack->PushLayer(layer); }
		inline void PushUILayer(Layer* layer) { m_UIlayerstack->PushLayer(layer); }
		inline void PopLayer(Layer* layer) { m_Entitylayerstack->PopLayer(layer); }
		inline void PopUILayer(Layer* layer) { m_UIlayerstack->PopLayer(layer); }
		//事件
		bool WindowClose(Event& e);
		bool WindowResize(Event& e);
		Scope<YaoWindow>& GetWindow() { return m_window; }
	protected:
		YaoEngine() ;
	private:	
		YaoEngine(const YaoEngine&) = delete;
		YaoEngine(YaoEngine&&) = delete;
		YaoEngine(YaoEngine&) = delete;
		Scope<YaoWindow> m_window;
		Scope<Layerstack> m_Entitylayerstack;
		Scope<Layerstack> m_UIlayerstack;
		static YaoEngine* m_instance;
		bool m_running = true;
	};
	YaoEngine* CreateApp();
}
