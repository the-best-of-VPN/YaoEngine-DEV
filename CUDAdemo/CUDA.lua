local cudaPath = os.getenv("CUDA_PATH") or "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.5"
local cudaBinPath = path.join(cudaPath, "bin")
local cudaIncludePath = path.join(cudaPath, "include")
local cudaLibPath = path.join(cudaPath, "lib/x64")
local nvccPath = path.join(cudaBinPath, "nvcc.exe")
local nvccCompileCommand = '"' .. nvccPath .. '" -c "%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.obj" -I"src" -I"' .. cudaIncludePath .. '" -std=c++20'

project "CUDA"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    staticruntime "On"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")

    files
    {
        "src/Header.cuh",
        "src/File.cu",
    }

    includedirs
    {
        "src",
        cudaIncludePath,
    }

    libdirs
    {
        cudaLibPath,
    }

    links
    {
        "cudart",
    }

    filter "files:**.cu"
        buildmessage "Compiling CUDA %{file.name}"

        buildoutputs
        {
            "%{cfg.objdir}/%{file.basename}.obj"
        }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter { "files:**.cu", "configurations:Debug" }
        buildcommands
        {
            nvccCompileCommand .. ' -D_DEBUG -Xcompiler "/MTd" -Xcompiler "/utf-8"'
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter { "files:**.cu", "configurations:Release" }
        buildcommands
        {
            nvccCompileCommand .. ' -DNDEBUG -Xcompiler "/MT" -Xcompiler "/utf-8"'
        }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"

    filter { "files:**.cu", "configurations:Dist" }
        buildcommands
        {
            nvccCompileCommand .. ' -DNDEBUG -Xcompiler "/MT" -Xcompiler "/utf-8"'
        }

    filter {}

project "CUDA-Demo"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    staticruntime "On"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")
    debugdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    debugenvs { "PATH=" .. cudaBinPath .. ";%PATH%" }

    files
    {
        "src/main.cpp",
    }

    includedirs
    {
        "src",
        cudaIncludePath,
    }

    libdirs
    {
        cudaLibPath,
    }

    links
    {
        "CUDA",
        "cudart",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"

    filter {}
