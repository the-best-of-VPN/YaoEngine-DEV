#include<pybind11/pybind11.h>



namespace YaoEngine {
	class PyScript {
	public:
		static bool init();
		static bool Detach();
	};
}