require("Premake/engine")

workspace("BrainFryer")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("On")
	flags("MultiProcessorCompile")

	startproject("BrainFryer")

	group("Libs")
	project("Engine")
		location("Engine/")
		warnings("Extra")
		libs.brainfryer:setup()
		common:addActions()

	group("Apps")
	project("Editor")
		location("Editor/")
		warnings("Extra")
		targetname("BrainfryerEditor")

		common:outDirs()
		common:debugDir()

		filter("configurations:Debug")
			kind("ConsoleApp")

		filter("configurations:not Debug")
			kind("WindowedApp")

		filter({})

		includedirs({ "%{prj.location}/Src/" })
		files({ "%{prj.location}/Src/**" })
		removefiles({ "*.DS_Store" })

		libs.brainfryer:setupDep()
		pkgdeps({
			"imgui-docking@latest",
			"stb@latest"
		})

		filter("system:windows")
			links({ "d3dcompiler.lib", "dwmapi.lib" })
		filter({})

		common:addActions()