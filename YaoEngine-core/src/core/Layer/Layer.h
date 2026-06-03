#pragma once
#include<string>
#include"../Event/Event.h"
/*
⌈–-−——————−——————−——————−——————−———————————⌉
|                                          |
|         ⌈–-−—————⌉                       |    Ui 层
|         |        |                       |	和photoshop那样一样图层先后渲染受
|         |        |                       |	事件
|         |        |                       |
|         ⌊–-−—————⌋                       |
|                                          |
|                                          |
|                                          |
|                                          |
|                                          |
⌊–-−——————−——————−——————−——————−———————————⌋
*/
namespace YaoEngine {
	class Layer {
	public:
		Layer(std::string name) {
			m_name = name;
		}
		virtual ~Layer() = default;
		virtual void OnUpdate(float ts) {};
		virtual void OnEvent(Event& e) {};
		virtual void OnAttach() {};
		virtual void DeAttach() {};
		virtual void OnImGuiRender(float ts) {};

	private:
		std::string  m_name;
	};
}