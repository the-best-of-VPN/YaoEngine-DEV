filter {}
project "ScriptCore"
    language "C#"
	dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"

    targetdir ("../build/bin/" .. outputDir .. "/%{prj.name}")
    objdir ("../build/intermediate/" .. outputDir .. "/%{prj.name}")

    files {
        "src/**.cs",
    }

filter {}