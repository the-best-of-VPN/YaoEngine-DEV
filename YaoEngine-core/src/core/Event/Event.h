#ifndef YAO_ENGINE_EVENT_H
#define YAO_ENGINE_EVENT_H
#include<string>
namespace YaoEngine {
	enum class EventType {
		None = 0,
		AppTick, AppUpdate, AppRender,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		KeyPressed, KeyReleased, KeyTyped
	};
	class Event {
	public:
		~Event() = default;
		virtual EventType GetEventType() const = 0;
		virtual std::string ToString() const = 0;
	};

#define EVENT(type)\
	static EventType GetStaticType() { return EventType::##type; }\
	virtual EventType GetEventType() const override { return GetStaticType(); }\
	virtual std::string ToString() const override { return #type; }
}
template<class  T,class Func>
class EventDispatcher {
	public:
	EventDispatcher(Event& event) :m_event(event) {}
	template<class  T>
	bool Dispatch(Func func) {
		if (m_event.GetEventType() == T::GetStaticType()) {
			func(static_cast<T&>(m_event));
			return true;
		}
		return false;
	}
private:
	Event m_event;
};
#endif