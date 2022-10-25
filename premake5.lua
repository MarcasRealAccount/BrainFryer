require("Premake/Common")

require("Premake/Libs/engine")

require("Premake/ThirdParty/glm")
require("Premake/ThirdParty/imgui")
require("Premake/ThirdParty/stb")
require("Premake/ThirdParty/entt")
require("Premake/ThirdParty/spdlog")

workspace("BrainFryer")
	common:setConfigsAndPlatforms()
	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("On")
	flags("MultiProcessorCompile")

	startproject("BrainFryer")

	group("Dependencies")
	project("STB")
		location("ThirdParty/STB/")
		warnings("Off")
		libs.stb:setup()
		location("ThirdParty/")

	project("GLM")
		location("ThirdParty/GLM/")
		warnings("Off")
		libs.glm:setup()
		location("ThirdParty/")

	project("EnTT")
		location("ThirdParty/EnTT/")
		warnings("Off")
		libs.entt:setup()
		location("ThirdParty/")

	project("ImGUI")
		location("ThirdParty/ImGUI/")
		warnings("Off")
		libs.imgui:setup()
		location("ThirdParty/")

	project("SpdLog")
		location("ThirdParty/SpdLog/")
		warnings("Off")
		libs.spdlog:setup()
		location("ThirdParty/")

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
		libs.imgui:setupDep()
		libs.stb:setupDep()

		filter("system:windows")
			links({ "d3dcompiler.lib" })
		filter({})

		common:addActions()