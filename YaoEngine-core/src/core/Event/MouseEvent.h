#pragma once
#include"./Event.h"
namespace YaoEngine {
	enum class MouseButton:int {
		None=0,
		ButtonLeft,
		ButtonRight,
		ButtonMiddle,
		Extra1,
		Extra2,
	};

	class MouseEvent :public Event {
	public:
		MouseEvent(const MouseButton& Mousecode= MouseButton::None) :m_keycode(Mousecode) {}
		MouseEvent(const MouseButton&&Mousecode=
			MouseButton::None) :m_keycode(Mousecode) {}
		~MouseEvent() = default;
	private:
		MouseButton m_keycode;
	};

	class MouseButtonPressedEvent : public MouseEvent {
	public:
		MouseButtonPressedEvent(const MouseButton& code = MouseButton::None) :MouseEvent(code) {};
		MouseButtonPressedEvent(const MouseButton&& code = MouseButton::None) :MouseEvent(code) {};
		~MouseButtonPressedEvent() = default;
		EVENT(MouseButtonPressed)
	private:

	};
	
	class MouseButtonReleasedEvent : public MouseEvent {
	public:
		MouseButtonReleasedEvent(const MouseButton &code=MouseButton::None) :MouseEvent(code) {};
		MouseButtonReleasedEvent(const MouseButton&& code=MouseButton::None) :MouseEvent(code) {};
		~MouseButtonReleasedEvent() = default;
		EVENT(MouseButtonReleased)
	private:
	};

	class MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(const int x,const int y) : m_mouseX(x), m_mouseY(y) {}
		~MouseMovedEvent() = default;
		EVENT(MouseMoved)
		int GetX() const { return m_mouseX; }
		int GetY() const { return m_mouseY; }
	private:
		int m_mouseX, m_mouseY;
	};

	class MouseScrolledEvent : public Event {
		MouseScrolledEvent(float xoffset, float yoffset) : m_xOffset(xoffset), m_yOffset(yoffset) {}
		~MouseScrolledEvent() = default;
		EVENT(MouseScrolled)
		float GetXOffset() const { return m_xOffset; }
		float GetYOffset() const { return m_yOffset; }
	private:
		float m_xOffset, m_yOffset;
	};
}