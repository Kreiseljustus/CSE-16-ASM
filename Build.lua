-- premake5.lua
workspace "CSE-16-ASM"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "CSE-16-ASM"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/permissive-", "/utf-8" }

OutputDir = "build/%{cfg.buildcfg}"

group "CSE-16-ASM"
	project "CSE-16-ASM"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++23"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp" }

   includedirs
   {
      "Source"
   }

   libdirs {

   }

   targetdir ("Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG"}
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE"}
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST"}
       runtime "Release"
       optimize "On"
       symbols "Off"
