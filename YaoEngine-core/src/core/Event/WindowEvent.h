#pragma once
#include"./Event.h"
namespace YaoEngine {
	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(int w, int h) : m_Width(w), m_Height(h) {}
		~WindowResizeEvent() = default;
		EVENT(WindowResize)
	private:
		unsigned int m_Width, m_Height;
	};

	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() = default;
		~WindowCloseEvent() = default;
		EVENT(WindowClose)
	};

	class WindowMoveEvent : public Event {
	public:
		WindowMoveEvent(int x, int y) : m_X(x), m_Y(y) {}
		~WindowMoveEvent() = default;
		EVENT(WindowMoved)
	private:
		int m_X, m_Y;
	};
}