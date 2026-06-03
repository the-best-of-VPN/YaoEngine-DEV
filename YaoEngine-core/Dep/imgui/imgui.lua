project "ImGui"

    location "ImGui"

    kind "StaticLib"
    language "C++"

    cppdialect "C++17"

    staticruntime "off"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")

    files
    {
        "imgui/*.h",
        "imgui/*.cpp",
    }

    includedirs
    {
        "imgui",
    }
