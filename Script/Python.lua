newoption {
    trigger = "python-home",
    value = "PATH",
    description = "CPython x64 SDK directory (defaults to YaoEngine-core/python)",
}

newoption {
    trigger = "python-smoke",
    description = "Generate the standalone Python runtime verification project",
}

local repositoryRoot = path.getabsolute("..", _SCRIPT_DIR)
local pythonRoot = path.getabsolute(
    _OPTIONS["python-home"] or os.getenv("YAO_PYTHON_HOME")
        or path.join(repositoryRoot, "YaoEngine-core/python"),
    repositoryRoot)

local function requireFile(relativePath)
    local filename = path.join(pythonRoot, relativePath)
    if not os.isfile(filename) then
        error("Incomplete Python SDK: " .. filename
            .. "\nProvide a full CPython x64 SDK with --python-home=PATH.", 0)
    end
    return filename
end

requireFile("include/Python.h")
requireFile("include/pyconfig.h")
local versionFile = assert(io.open(requireFile("include/patchlevel.h"), "r"))
local versionHeader = versionFile:read("*a")
versionFile:close()
local major = tonumber(versionHeader:match("#define%s+PY_MAJOR_VERSION%s+(%d+)"))
local minor = tonumber(versionHeader:match("#define%s+PY_MINOR_VERSION%s+(%d+)"))
if major ~= 3 or not minor or minor < 9 then
    error("pybind11 requires CPython 3.9 or newer: " .. pythonRoot, 0)
end

local libraryName = "python" .. major .. minor
requireFile("libs/" .. libraryName .. ".lib")
requireFile("Lib/encodings/__init__.py")
local runtimeDll = requireFile(libraryName .. ".dll")

-- The workspace is x64; reject an x86/ARM SDK before generating projects.
local dllFile = assert(io.open(runtimeDll, "rb"))
local dosHeader = dllFile:read(64)
local machine
if dosHeader and #dosHeader == 64 and dosHeader:sub(1, 2) == "MZ" then
    local a, b, c, d = dosHeader:byte(61, 64)
    dllFile:seek("set", a + b * 256 + c * 65536 + d * 16777216)
    local peHeader = dllFile:read(6)
    if peHeader and #peHeader == 6 and peHeader:sub(1, 4) == "PE\0\0" then
        local low, high = peHeader:byte(5, 6)
        machine = low + high * 256
    end
end
dllFile:close()
if machine ~= 0x8664 then
    error("The YaoEngine x64 workspace requires an x64 Python DLL: " .. runtimeDll, 0)
end

local sdk = {
    root = pythonRoot,
    library = libraryName,
}

function sdk.configure()
    includedirs {
        path.join(repositoryRoot, "YaoEngine-core/Dep/pybind11/include"),
        path.join(pythonRoot, "include"),
    }
    libdirs { path.join(pythonRoot, "libs") }
    -- All engine configurations use regular CPython, including Debug.
    links { libraryName }
    dependson { "pybind11" }
    buildoptions { "/utf-8" }
end

function sdk.deploy()
    -- Keep Python's Lib separate from Mono's lib on Windows.
    postbuildcommands {
        '{COPYDIR} "' .. path.join(pythonRoot, "Lib")
            .. '" "%{cfg.buildtarget.directory}/python/Lib"',
        '{COPYFILE} "' .. path.join(repositoryRoot, "Script/python._pth")
            .. '" "%{cfg.buildtarget.directory}/' .. libraryName .. '._pth"',
    }
    if os.isdir(path.join(pythonRoot, "DLLs")) then
        postbuildcommands {
            '{COPYDIR} "' .. path.join(pythonRoot, "DLLs")
                .. '" "%{cfg.buildtarget.directory}/python/DLLs"',
        }
    end
    for _, dll in ipairs(os.matchfiles(path.join(pythonRoot, "*.dll"))) do
        postbuildcommands {
            '{COPYFILE} "' .. dll .. '" "%{cfg.buildtarget.directory}"',
        }
    end
    if os.isfile(path.join(pythonRoot, "LICENSE.txt")) then
        postbuildcommands {
            '{COPYFILE} "' .. path.join(pythonRoot, "LICENSE.txt")
                .. '" "%{cfg.buildtarget.directory}/python/LICENSE.txt"',
        }
    end
end

return sdk
