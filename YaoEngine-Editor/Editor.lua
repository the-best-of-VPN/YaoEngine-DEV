project "YaoEngine-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")
    debugdir "$(SolutionDir)"

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
    }

    includedirs{
        "../YaoEngine-core/src",

    }

    links{
        "YaoEngine-core",
    }

    defines{
        "STD_SMART_PTR",
    }

    filter "system:windows"
        systemversion "latest"
    
