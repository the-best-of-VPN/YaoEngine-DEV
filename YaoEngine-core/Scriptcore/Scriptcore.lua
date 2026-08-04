filter {}
project "ScriptCore"
    language "C#"
	dotnetframework "4.7.2"
    kind "SharedLib"
	clr "Unsafe"

    targetdir ("%{wks.location}/build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/build/intermediate/" .. outputDir .. "/%{prj.name}")

    files {
        "src/**.cs",
    }

filter {}
