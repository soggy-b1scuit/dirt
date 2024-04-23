---@diagnostic disable: undefined-global

workspace "Engine"
    configurations { "debug", "release" }
    platforms      { "x86_64", "x64" }

    filter "configurations:debug"
        runtime  "debug"
        symbols  "on"
        optimize "off"
    filter "configurations:release"
        runtime  "release"
        symbols  "off"
        optimize "on"
    filter "platforms:x86_64"
        architecture "x86_64"
    filter "platforms:x64"
        architecture "x64"

project "dirt"
    kind          "ConsoleApp"
    language      "C++"
    cppdialect    "C++20"
    staticruntime "on"
    systemversion "latest"

    targetname "dirt"

    targetdir     ""
    objdir        "obj/%{cfg.buildcfg}"

    files { "src/**.h", "src/**.c", "src/**.hpp", "src/**.cpp" }

    links { "raylib" }
    libdirs { "libs" }

    includedirs
    {
        "include",
        "src/backend/miniaudio",
    }

    filter "system:windows"
        links { "gdi32", "user32", "kernel32", "ole32", "oleaut32", "imm32", "winmm", "Comdlg32" }

    disablewarnings {"unused-function", "return-type"}