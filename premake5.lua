workspace "hill"
	architecture "x86_64"
	startproject "hill"

	configurations {
		"debug",
		"release",
	}

project "hill"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	warnings "Extra"
	fatalwarnings "All"

	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	objdir "%{wks.location}/bin-int/%{cfg.buildcfg}"

	files {
		"src/**.hpp",
		"src/**.cpp",
	}

	includedirs {
		"src",
	}

	filter "configurations:debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:release"
		runtime "Release"
		optimize "on"
