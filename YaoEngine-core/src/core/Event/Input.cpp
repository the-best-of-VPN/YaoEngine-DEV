#include "Input.h"
#include "YaoEngine.h"   
#include <GLFW/glfw3.h>
#include<array>
namespace YaoEngine {

	namespace {
		
		struct GamepadSnapshot {
			bool  connected = false;
			std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> buttons{};
			std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1>  axes{};
		};

	
		struct InputSnapshot {
			std::array<bool, GLFW_KEY_LAST + 1>                 keys{};
			std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>        mouseButtons{};
			std::array<GamepadSnapshot, GLFW_JOYSTICK_LAST + 1> gamepads{};
		};

		InputSnapshot s_Current;
		InputSnapshot s_Previous;

		GLFWwindow* GetWindow() {
			return static_cast<GLFWwindow*>(YaoEngine::Getinstance()->GetWindow()->GetNativeWindow());
		}
	}

	void Input::Init() {
		
	}

	void Input::OnUpdate() {
		
		s_Previous = s_Current;

		GLFWwindow* window = GetWindow();

		for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
			s_Current.keys[key] = glfwGetKey(window, key) == GLFW_PRESS;


		for (int btn = 0; btn <= GLFW_MOUSE_BUTTON_LAST; ++btn)
			s_Current.mouseButtons[btn] = glfwGetMouseButton(window, btn) == GLFW_PRESS;

	
		for (int jid = 0; jid <= GLFW_JOYSTICK_LAST; ++jid) {
			GamepadSnapshot& pad = s_Current.gamepads[jid];
			GLFWgamepadstate state;

			if (glfwJoystickIsGamepad(jid) && glfwGetGamepadState(jid, &state)) {
				pad.connected = true;
				for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; ++b)
					pad.buttons[b] = state.buttons[b] == GLFW_PRESS;
				for (int a = 0; a <= GLFW_GAMEPAD_AXIS_LAST; ++a)
					pad.axes[a] = state.axes[a];
			}
			else {
				pad = GamepadSnapshot{}; 
			}
		}
	}

	bool Input::IsKeyPressed(Keycode key) {
		int k = static_cast<int>(key);
		if (k < GLFW_KEY_SPACE || k > GLFW_KEY_LAST) return false;
		return s_Current.keys[k];
	}
	bool Input::IsKeyDown(Keycode key) {
		int k = static_cast<int>(key);
		if (k < GLFW_KEY_SPACE || k > GLFW_KEY_LAST) return false;
		return s_Current.keys[k] && !s_Previous.keys[k];
	}
	bool Input::IsKeyReleased(Keycode key) {
		int k = static_cast<int>(key);
		if (k < GLFW_KEY_SPACE || k > GLFW_KEY_LAST) return false;
		return !s_Current.keys[k] && s_Previous.keys[k];
	}

	bool Input::IsMouseButtonPressed(MouseButton button) {
		int b = static_cast<int>(button);
		if (b < 0 || b > GLFW_MOUSE_BUTTON_LAST) return false;
		return s_Current.mouseButtons[b];
	}
	bool Input::IsMouseButtonDown(MouseButton button) {
		int b = static_cast<int>(button);
		if (b < 0 || b > GLFW_MOUSE_BUTTON_LAST) return false;
		return s_Current.mouseButtons[b] && !s_Previous.mouseButtons[b];
	}
	bool Input::IsMouseButtonReleased(MouseButton button) {
		int b = static_cast<int>(button);
		if (b < 0 || b > GLFW_MOUSE_BUTTON_LAST) return false;
		return !s_Current.mouseButtons[b] && s_Previous.mouseButtons[b];
	}
	std::pair<float, float> Input::GetMousePosition() {
		double x, y;
		glfwGetCursorPos(GetWindow(), &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}
	float Input::GetMouseX() { return GetMousePosition().first; }
	float Input::GetMouseY() { return GetMousePosition().second; }

	
	bool Input::IsGamepadConnected(Joystick joystick) {
		int jid = static_cast<int>(joystick);
		if (jid < 0 || jid > GLFW_JOYSTICK_LAST) return false;
		return s_Current.gamepads[jid].connected;
	}
	bool Input::IsGamepadButtonPressed(Joystick joystick, GamepadButton button) {
		int jid = static_cast<int>(joystick), b = static_cast<int>(button);
		if (jid < 0 || jid > GLFW_JOYSTICK_LAST) return false;
		if (b < 0 || b > GLFW_GAMEPAD_BUTTON_LAST) return false;
		return s_Current.gamepads[jid].buttons[b];
	}
	bool Input::IsGamepadButtonDown(Joystick joystick, GamepadButton button) {
		int jid = static_cast<int>(joystick), b = static_cast<int>(button);
		if (jid < 0 || jid > GLFW_JOYSTICK_LAST) return false;
		if (b < 0 || b > GLFW_GAMEPAD_BUTTON_LAST) return false;
		return s_Current.gamepads[jid].buttons[b] && !s_Previous.gamepads[jid].buttons[b];
	}
	bool Input::IsGamepadButtonReleased(Joystick joystick, GamepadButton button) {
		int jid = static_cast<int>(joystick), b = static_cast<int>(button);
		if (jid < 0 || jid > GLFW_JOYSTICK_LAST) return false;
		if (b < 0 || b > GLFW_GAMEPAD_BUTTON_LAST) return false;
		return !s_Current.gamepads[jid].buttons[b] && s_Previous.gamepads[jid].buttons[b];
	}
	float Input::GetGamepadAxis(Joystick joystick, GamepadAxis axis) {
		int jid = static_cast<int>(joystick), a = static_cast<int>(axis);
		if (jid < 0 || jid > GLFW_JOYSTICK_LAST) return 0.0f;
		if (a < 0 || a > GLFW_GAMEPAD_AXIS_LAST) return 0.0f;
		return s_Current.gamepads[jid].axes[a];
	}
	float Input::GetGamepadTrigger(Joystick joystick, GamepadAxis axis) {
		return (GetGamepadAxis(joystick, axis) + 1.0f) * 0.5f; 
	}
}