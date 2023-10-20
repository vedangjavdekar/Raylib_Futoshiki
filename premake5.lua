include "Dependencies.lua"

workspace "Futoshiki"
   configurations { "Debug", "Release"}
   architecture "x86_64"

versionPath = "version%{Version.Major}_%{Version.Minor}/" 
projectLocation = "%{wks.location}/build/%{prj.name}"
basedir = "%{wks.location}/"
outputdir = "%{cfg.buildcfg}"
targetPath = basedir .. "bin/" .. outputdir
objectPath = basedir .. "bin-int/" .. outputdir 
debugPath =  basedir 

group "Dependencies"
   include "Engine/vendor/raylib"
group ""

group "Core"
   include "Engine"
group ""