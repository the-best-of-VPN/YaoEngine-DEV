project "CUDA"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    staticruntime "off"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "src/**.cuh",
        "src/**.cu"
    }

    includedirs
    {
        "src",

        -- CUDA SDK
        "D:\\cuda\\include",
    }

    libdirs
    {
        "D:\\cuda\\lib\\x64",
    }

    links
    {
        "cudart",
    }

    filter "files:**.cu"

        buildmessage "Compiling CUDA %{file.name}"

        buildcommands
        {
            [[
            "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.5/bin/nvcc.exe"
            -c %{file.relpath}
            -o %{cfg.objdir}/%{file.basename}.obj
            -I"src"
            -std=c++20
            ]]
        }

        buildoutputs
        {
            "%{cfg.objdir}/%{file.basename}.obj"
        }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter {}