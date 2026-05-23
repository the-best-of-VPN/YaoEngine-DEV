project "YaoEngine-core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
    }

    includedirs{
        "src",
        "Dep/GLFW/include",
    }


    links{
        "GLFW",
    }

    defines{
        "STD_SMART_PTR",
    }
