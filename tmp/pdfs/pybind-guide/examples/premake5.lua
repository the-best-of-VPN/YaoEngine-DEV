newoption { trigger = "python-home", value = "PATH",
    description = "Full CPython installation directory" }
newoption { trigger = "python-lib", value = "NAME",
    description = "Python import library, e.g. python312" }

local pyhome = _OPTIONS["python-home"]
local pylib = _OPTIONS["python-lib"]
assert(pyhome and pylib, "Set --python-home and --python-lib")
pyhome = path.getabsolute(pyhome)

workspace "PybindDemo"
    architecture "x64"
    configurations { "Release" }
    location "build"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    runtime "Release"
    buildoptions { "/utf-8" }
    symbols "On"
    optimize "On"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{prj.name}/%{cfg.buildcfg}"
    includedirs {
        "../../../../YaoEngine-core/Dep/pybind11/include",
        path.join(pyhome, "include"),
    }
    libdirs { path.join(pyhome, "libs") }
    links { pylib }

project "yao_math"
    kind "SharedLib"
    targetprefix ""
    targetextension ".pyd"
    files { "bindings.cpp" }

project "py_runner"
    kind "ConsoleApp"
    files { "main.cpp" }
    debugdir (path.getabsolute("."))
    debugenvs { "PATH=" .. pyhome .. ";%PATH%" }

project "engine_demo"
    kind "ConsoleApp"
    files { "engine.cpp" }
    debugdir (path.getabsolute("."))
    debugenvs { "PATH=" .. pyhome .. ";%PATH%" }
