#pragma once
#include"Layer.h"
namespace YaoEngine {
	class imguiimport :public Layer
	{
	public:
		imguiimport(std::string name = "imguiimport") :Layer(name) {};
		virtual ~imguiimport() = default;
		virtual void OnUpdate() override {};
		virtual void OnEvent(Event& e) override {};
		virtual void OnImGuiRender() override {};
		virtual void OnAttach() override {};
		virtual void DeAttach() override {};

	};
}