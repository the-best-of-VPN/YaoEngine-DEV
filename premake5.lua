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
    
    group "YaoEngine-core Dependencies"
        include "YaoEngine-core/Dep/GLFW/GLFW.lua"
        include "CUDAdemo/CUDA.lua"
        include"YaoEngine-core/Scriptcore/Scriptcore.lua"
        include"YaoEngine-core/Dep/imgui/imgui.lua"
    group ""
    group "Scripting"
      
    group""