-- specify custom premake5 options	
newoption {
	trigger = "app-type",
	value = "AppType",
	description = "Choose a app type",
	allowed = {
	    { "windowed",    "Windowed App" },
	    { "console",  "Console App" },
	}
}
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------

-- solution settings
workspace "Nitro"
	architecture "x64"
	startproject "Nitro.Sandbox"

	configurations
	{
		"debug", 
		"release",	-- not traditional release, just faster version of Debug
		"dist"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}/"

-- make a struct to include directories relative to sln folder
IncludeDir 				= {}
IncludeDir["GLFW"] 		= "Nitro/vendor/GLFW/include"
IncludeDir["RapidXML"] 	= "Nitro/vendor/rapidxml"	-- V1.13
IncludeDir["GLAD"] 		= "Nitro/vendor/GLAD/include"	-- Core/GL:V4.6
IncludeDir["ImGui"] 	= "Nitro/vendor/imgui"
IncludeDir["fmtlib"] 	= "Nitro/vendor/fmtlib/include"
IncludeDir["glm"] 		= "Nitro/vendor/glm"
-- include non-vendor projects
IncludeDir["Nitro_Physics"] = "Nitro.Physics/include"
IncludeDir["Nitro_Math"] = "Nitro.Math/src"
-- include compiled shaders into includedirs
IncludeDir["outputDir"] = "bin/"..outputdir.."%{prj.name}"

--include the premake5.lua within 'Nitro/vendor/GLFW'
include "Nitro/vendor/GLFW"
include "Nitro/vendor/GLAD"
include "Nitro/vendor/imgui"
include "Nitro/vendor/rapidxml"
include "Nitro.Physics"
include "Nitro.Math"

project "Nitro"
	location "Nitro"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/"..outputdir.."%{prj.name}")
	objdir ("bin_intermediate/"..outputdir.."%{prj.name}")

	pchheader "NtPCH.h"
	pchsource "Nitro/src/NtPCH.cpp"

	shadermodel "5.1"
	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		------ below should be removed as it's in other project  ------
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	links	-- projs wihtin will be compiled as static libraries.
	{
		-- vendor projs
		"GLFW",
		"GLAD",
		"ImGui",
		-- non-vendor projs
		"Nitro.Physics",
		-- libraries
		"opengl32.lib",
		"d3d12.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.RapidXML}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.fmtlib}",
		"%{IncludeDir.outputDir}",
		"%{IncludeDir.glm}",
		-- include non-vendor projects
		"%{IncludeDir.Nitro_Physics}",
		"%{IncludeDir.Nitro_Math}",
	}

	defines
	{
		-------- RSG MACRO SETTING -------
		"__FINAL=0",

		-------- RSG Utilities --------
		"Likely(x)=x",
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"NT_PLATFORM_WINDOWS",
			"NT_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

	filter {"system:windows", "options:app-type=console"}
		defines "NT_CONSOLE_APP"
		
	filter {"system:windows", "options:app-type=windowed"}
		defines "NT_WINDOWED_APP"
		files 
		{
			"%{prj.name}/src/**.hlsl",
			"%{prj.name}/src/**.hlsli"
		}

	filter "files:**.hlsl"
		flags "ExcludeFromBuild"
		shaderheaderfileoutput ("../bin/"..outputdir.."%{prj.name}/".."CompiledShaders/Headers/".."%{file.basename}"..".h")
		shadervariablename ("gp_".."%{file.basename}")
		shaderobjectfileoutput ("../bin/"..outputdir.."%{prj.name}/".."CompiledShaders/Objects/".."%{file.basename}"..".cso")
		-- shaderassembleroutput(shader_dir.."%{file.basename}"..".asm")

	filter "files:**_vs.hlsl"
		removeflags "ExcludeFromBuild"
		shadertype "Vertex"

	filter "files:**_ps.hlsl"
		removeflags "ExcludeFromBuild"
		shadertype "Pixel"

	filter "configurations:debug"
		defines "NT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:release"
		defines "NT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:dist"
		defines "NT_DIST"
		runtime "Release"
		optimize "on"


project "Nitro.Sandbox"
	location "Nitro.Sandbox"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/"..outputdir.."%{prj.name}")
	objdir ("bin_intermediate/"..outputdir.."%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Nitro/vendor/spdlog/include",
		"Nitro/src",
		"Nitro/vendor",
		"%{IncludeDir.RapidXML}",
		"%{IncludeDir.glm}",
		-- include non-vendor projects
		"%{IncludeDir.Nitro_Physics}",
		"%{IncludeDir.Nitro_Math}",
	}

	links
	{
		"Nitro",
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"NT_PLATFORM_WINDOWS"
		}

	filter {"system:windows", "options:app-type=console"}
		defines "NT_CONSOLE_APP"
		kind "ConsoleApp"
		files 
		{
			"%{prj.name}/assets/shaders/gl/**.glsl",
		}

	filter {"system:windows", "options:app-type=windowed"}
		defines "NT_WINDOWED_APP"
		kind "WindowedApp"

	filter "configurations:debug"
		defines "NT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:release"
		defines "NT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:dist"
		defines "NT_DIST"
		runtime "Release"
		optimize "on"