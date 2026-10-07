YaoPython = dofile("Script/Python.lua")

workspace "YaoEngine"
    platforms {
        "x64",
    }
    architecture(
        "x64")
    
    configurations { "Debug", 
                    "Release",
                    "Dist" }

    startproject "YaoEngine-Editor"

    outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"

    filter "configurations:Dist"
        defines { "DIST" }
        optimize "On"

    filter {}
    


    group "Core"
        include "YaoEngine-core/core.lua"
    group ""

    group "Editor"
        include "YaoEngine-Editor/Editor.lua"
    group ""

    group "Applications"
        include "YaoEngine-runtime/runtime.lua"
        include "YaoEngine-Launcher/Launther.lua"
    group ""
    
    group "YaoEngine-core Dependencies"
        include "YaoEngine-core/Dep/GLFW/GLFW.lua"
        include "CUDAdemo/CUDA.lua"
        include"YaoEngine-core/Scriptcore/Scriptcore.lua"
        include"YaoEngine-core/Dep/imgui/imgui.lua"
        include "YaoEngine-core/Dep/pybind11/pybind11.lua"
    group ""
    group "Scripting"
        if _OPTIONS["python-smoke"] then
            include "Script/PythonSmoke/PythonSmoke.lua"
        end
    group""
