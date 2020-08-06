workspace "XYZ_Engine"
		architecture "x64"
		startproject "XYZEditor"

		configurations
		{
				"Debug",
				"Release"
		}

		flags
		{
			"MultiProcessorCompile"
		}
		
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "XYZ_Engine/vendor/GLFW/include"
IncludeDir["GLEW"] = "XYZ_Engine/vendor/GLEW/include"
IncludeDir["glm"] = "XYZ_Engine/vendor/glm"
IncludeDir["mini"] = "XYZ_Engine/vendor/mini"
IncludeDir["OpenAL"] = "XYZ_Engine/vendor/OpenAL-Soft"
IncludeDir["MiniMp3"] = "XYZ_Engine/vendor/minimp3"


include "XYZ_Engine/vendor/GLFW"
include "XYZ_Engine/vendor/GLEW"
include "XYZ_Engine/vendor/OpenAL-Soft"

project "XYZ_Engine"
		location "XYZ_Engine"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		pchheader "stdafx.h"
		pchsource "XYZ_Engine/src/stdafx.cpp"

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/vendor/glm/glm/**.hpp",
			"%{prj.name}/vendor/glm/glm/**.inl",
			"%{prj.name}/vendor/mini/ini.h",
			"%{prj.name}/vendor/stb_image/**.h",
			"%{prj.name}/vendor/stb_image/**.cpp",

			"%{prj.name}/vendor/yaml-cpp/src/**.cpp",
			"%{prj.name}/vendor/yaml-cpp/src/**.h",
			"%{prj.name}/vendor/yaml-cpp/include/**.h"

		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"GLFW_INCLUDE_NONE",
			"GLEW_STATIC",
			"AL_LIBTYPE_STATIC"	
		}

		includedirs
		{
			"%{prj.name}/src",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.GLEW}",
			"%{IncludeDir.glm}",
			"%{IncludeDir.mini}",
			"%{IncludeDir.OpenAL}/include",
			"%{IncludeDir.OpenAL}/src",
			"%{IncludeDir.OpenAL}/src/common",
			"%{IncludeDir.MiniMp3}",		
			"%{prj.name}/vendor/stb_image",
			"%{prj.name}/vendor/yaml-cpp/include"
		}

		links
		{
			"GLEW",
			"GLFW",
			"OpenAL-Soft",
			"opengl32"
		}
		flags { "NoPCH" }
		
		filter "system:windows"
				systemversion "latest"


		filter "configurations:Debug"
				defines "XYZ_DEBUG"
				runtime "Debug"
				symbols "on"


		filter "configurations:Release"
				defines "XYZ_RELEASE"
				runtime "Release"
				optimize "on"

project "XYZEditor"
		location "XYZEditor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"XYZ_Engine/vendor",
			"XYZ_Engine/src",
			"%{IncludeDir.glm}"
		}

		links
		{
			"XYZ_Engine"
		}

		filter "system:windows"
				systemversion "latest"

		filter "configurations:Debug"
				defines "XYZ_DEBUG"
				runtime "Debug"
				symbols "on"

		filter "configurations:Release"
				defines "XYZ_RELEASE"
				runtime "Release"
				optimize "on"


project "ZSandbox"
		location "ZSandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"
		
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
		
		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}
				
		includedirs
		{
			"XYZ_Engine/vendor",
			"XYZ_Engine/src",
			"%{IncludeDir.glm}"
		}

		links
		{
			"XYZ_Engine"
		}

		filter "system:windows"
				systemversion "latest"

		filter "configurations:Debug"
				defines "XYZ_DEBUG"
				runtime "Debug"
				symbols "on"

		filter "configurations:Release"
				defines "XYZ_RELEASE"
				runtime "Release"
				optimize "on"				
				


