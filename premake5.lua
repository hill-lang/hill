workspace "hill"
	architecture "x86_64"
	startproject "hill"

	configurations {
		"Debug",
		"Release",
		"Dist",
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	include "hill"
