#include <pybind11/embed.h>
#include <iostream>
namespace py = pybind11;

struct Actor {
    float x = 0.0f;
    void move(float dx) { x += dx; }
};

PYBIND11_EMBEDDED_MODULE(yao, m) {
    py::class_<Actor, py::smart_holder>(m, "Actor")
        .def(py::init<>())
        .def_readwrite("x", &Actor::x)
        .def("move", &Actor::move);
}

int main() {
    py::scoped_interpreter python{};
    try {
        auto sys = py::module_::import("sys");
        sys.attr("path").attr("insert")(0, "scripts");
        auto yao = py::module_::import("yao");
        auto actor = yao.attr("Actor")();
        auto update = py::module_::import("behavior")
                          .attr("on_update");
        for (int frame = 0; frame < 3; ++frame)
            update(actor, 0.5f);
        std::cout << actor.attr("x").cast<float>()
                  << '\n';
    } catch (const py::error_already_set &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
