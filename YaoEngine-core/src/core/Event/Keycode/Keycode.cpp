#include"Keycode.h"
namespace YaoEngine {
	Keycode Utill::GLFWkeycodetoYaokeycode(int keycode) {
		return static_cast<Keycode>(keycode);
	}
}