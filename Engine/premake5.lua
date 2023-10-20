project "Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir(targetPath)
    objdir(objectPath)
    location(projectLocation)
    debugdir(debugPath)

    dependson {
       "raylib"
    }

    links{
        "raylib"
    }

    files { 
        "src/**.h",
        "src/**.cpp"
    }

    defines{
        "BUILD_LIBTYPE_SHARED"
    }

    includedirs {
        "src",
        "%{IncludeDirs.raylib}",
    }

    filter "configurations:Debug"
        defines { "BUILD_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "BUILD_RELEASE" }
        optimize "On"