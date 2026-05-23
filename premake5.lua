workspace "YaoEngine"
    platforms {
        "x86_64",
        "x86",
        "arm64",
        "arm",
    }
    
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
    group ""