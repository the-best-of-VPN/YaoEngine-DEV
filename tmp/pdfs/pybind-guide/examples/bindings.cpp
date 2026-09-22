#include <pybind11/pybind11.h>
namespace py = pybind11;

float advance(float x, float speed, float dt) {
    return x + speed * dt;
}

PYBIND11_MODULE(yao_math, m) {
    m.doc() = "Small math helpers for YaoEngine";
    m.def("advance", &advance,
          py::arg("x"), py::arg("speed"),
          py::arg("dt") = 1.0f / 60.0f);
}
