#include <pybind11/embed.h>
#include <iostream>
namespace py = pybind11;

int main() {
    py::scoped_interpreter python{};
    try {
        auto sys = py::module_::import("sys");
        sys.attr("path").attr("insert")(0, "scripts");
        auto script = py::module_::import("movement");
        float x = script.attr("update")(2.0f, 0.5f)
                        .cast<float>();
        std::cout << "x = " << x << '\n';
    } catch (const py::error_already_set &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
