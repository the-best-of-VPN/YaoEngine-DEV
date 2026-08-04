project "YaoEngine-core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

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
    }
    includedirs{
        "src",
        "src/core",
        "Dep/GLFW/include",
        "Dep/glad/include",
        "Dep/glm",
        "Dep/imgui/imgui",
         "%{MonoPath}/include/mono-2.0",
         "../CUDAdemo/src",
    }
    links{
        "GLFW",
        "CUDA",
        "ImGui",
         "mono-2.0-sgen",
    }

    defines{
        "STD_SMART_PTR",
        "GLFW_Window",
    }

    libdirs{
        "%{MonoPath}/lib"
    }

