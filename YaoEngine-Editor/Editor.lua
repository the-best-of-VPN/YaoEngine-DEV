project "YaoEngine-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    local cudaPath = os.getenv("CUDA_PATH") or "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.5"
    local cudaBinPath = path.join(cudaPath, "bin")
    local cudaLibPath = path.join(cudaPath, "lib/x64")
    local monoBinPath = path.getabsolute("../YaoEngine-core/Dep/Mono/bin")

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")
    debugdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    debugenvs { "PATH=" .. cudaBinPath .. ";" .. monoBinPath .. ";%PATH%" }

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
    }
postbuildcommands {
    '{COPYDIR} "../YaoEngine-core/Dep/Mono/bin" "%{cfg.buildtarget.directory}"',
    '{COPYDIR} "../YaoEngine-core/Dep/Mono/etc" "%{cfg.buildtarget.directory}/etc"',
    '{COPYDIR} "../YaoEngine-core/Dep/Mono/lib" "%{cfg.buildtarget.directory}/lib"'
}
    includedirs{
        "../YaoEngine-core/src",
        "../YaoEngine-core/Dep/glad/include",
        "../YaoEngine-core/Dep/glm",
        "../YaoEngine-core/Dep/imgui/imgui",
    }

    links{
        "YaoEngine-core",
        "CUDA",
        "cudart",
    }

    libdirs{
        cudaLibPath,
    }

    defines{
        "STD_SMART_PTR",
    }

    filter "system:windows"
        systemversion "latest"
   
