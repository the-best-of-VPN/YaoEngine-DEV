project "PythonSmoke"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("../../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../../build/intermediate/" .. outputDir .. "/%{prj.name}")
    debugdir "%{cfg.buildtarget.directory}"
    files { "main.cpp" }

    YaoPython.configure()
    YaoPython.deploy()

    filter "configurations:Debug"
        runtime "Debug"
    filter "configurations:Release or Dist"
        runtime "Release"
    filter {}
