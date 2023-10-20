project "raylib"
    kind "SharedLib"

    defines {
        "BUILD_LIBTYPE_SHARED",
        "GRAPHICS_API_OPENGL_43",
        "PLATFORM_DESKTOP"
    }
    
    language "C++"

    location(projectLocation)
    targetdir(targetPath)
    objdir(objectPath)
    debugdir(debugPath)
    
    includedirs { 
        "include", 
        "src/external/",
        "src/external/glfw/include",
        "src/extras" 
    }

    vpaths 
    {
        ["Header Files"] = { "src/**.h"},
        ["Source Files/*"] = {"src/**.c"},
    }

    files {
        "src/*.h", 
        "src/*.c"
    }

    filter "action:vs*"
        defines{
            "_WINSOCK_DEPRECATED_NO_WARNINGS",
            "_CRT_SECURE_NO_WARNINGS"
        }
        characterset ("MBCS")
    
    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm"}
        
    filter "system:linux"
        links {
            "pthread", 
            "GL", 
            "m", 
            "dl", 
            "rt", 
            "X11"
        }
        
    filter{}
    