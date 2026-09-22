#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(yao_python_smoke, module) {
    module.def("advance", [](float x, float speed, float dt) {
        return x + speed * dt;
    });
}

int main() {
    py::scoped_interpreter interpreter{};
    try {
        // Validate the deployed standard library and native DLL dependencies.
        py::exec(R"(
import encodings, json, socket, ssl, sqlite3, sys
from pathlib import Path
import yao_python_smoke
assert sys.flags.isolated == 1
expected_stdlib = Path(sys.executable).resolve().parent / "python" / "Lib"
assert Path(encodings.__file__).resolve().parent.parent == expected_stdlib
assert json.loads('{"ready": true}')["ready"]
assert yao_python_smoke.advance(2.0, 4.0, 0.5) == 4.0
with sqlite3.connect(":memory:") as db:
    assert db.execute("SELECT 42").fetchone()[0] == 42
print("Python:", sys.version.split()[0])
print("Prefix:", sys.prefix)
print("Standard library:", encodings.__file__)
print("OpenSSL:", ssl.OPENSSL_VERSION)
print("Python smoke passed", flush=True)
)");
    } catch (const py::error_already_set& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
