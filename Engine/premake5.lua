project "Engine"
    language "C++"
    cppdialect "C++17"

    filter "configurations:Debug"
        kind "ConsoleApp"
    filter "configurations:Release"
        kind "WindowedApp"
    filter {}

    targetdir(targetPath)
    objdir(objectPath)
    location(projectLocation)
    debugdir(debugPath)

    dependson {
       "raylib",
    }

    links{
        "raylib",
    }

    files { 
        "src/**.h",
        "src/**.cpp"
    }

    defines{
        "BUILD_LIBTYPE_SHARED",
        "FMT_HEADER_ONLY"
    }

    includedirs {
        "src",
        "%{IncludeDirs.raylib}",
        "%{IncludeDirs.fmt}",
    }

    filter "configurations:Debug"
        defines { "BUILD_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "BUILD_RELEASE" }
        optimize "On"