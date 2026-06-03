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
 debugdir ("../build/bin/" .. outputDir .. "/%{prj.name}")

postbuildcommands {
    '{COPYDIR} "%{wks.location}YaoEngine-core/Dep/Mono/lib" "%{cfg.buildtarget.directory}/lib"'
}
    includedirs{
        "../YaoEngine-core/src",
        "../YaoEngine-core/Dep/glad",
    }

    links{
        "YaoEngine-core",
    }

    defines{
        "STD_SMART_PTR",
    }

    filter "system:windows"
        systemversion "latest"
   
