project "hill"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	objdir "%{wks.location}/bin-int/%{cfg.buildcfg}"

	files {
		"src/**.hpp",
		"src/**.cpp",
	}

	includedirs {
		"src",
	}

	filter "configurations:Debug"
		defines "HILL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HILL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HILL_DIST"
		runtime "Release"
		optimize "on"
