project "YaoEngine-core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    editandcontinue "Off"
    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")
    MonoPath = "Dep/Mono"

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
        "Dep/glad/**.c",
        "Dep/glad/**.cpp",
        "Dep/glad/**.h",
        "Shader.yao/**.yao",
        "Dep/tracy/public/TracyClient.cpp",
    }
    includedirs{
        "src",
        "src/core",
        "Dep/GLFW/include",
        "Dep/glad/include",
        "Dep/glm",
        "Dep/imgui/imgui",
        "Dep/tracy/public",
         "%{MonoPath}/include/mono-2.0",
         "../CUDAdemo/src",
    }
    links{
        "GLFW",
        "CUDA",
        "ImGui",
         "mono-2.0-sgen",
    }

    YaoPython.configure()

    defines{
        "STD_SMART_PTR",
        "GLFW_Window",
        "TRACY_ENABLE",
    }

    libdirs{
        "%{MonoPath}/lib"
    }

