libs            = libs            or {}
libs.brainfryer = libs.brainfryer or {
	name       = "",
	location   = ""
}

require("../ThirdParty/glm")
require("../ThirdParty/entt")
require("../ThirdParty/spdlog")

local brainfryer = libs.brainfryer

function brainfryer:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/Inc/" })

	files({
		self.location .. "/Inc/**",
		self.location .. "/Src/**"
	})

	filter("system:windows")
		links({ "d3d12.lib", "dxgi.lib", "Dbghelp.lib" })
	filter({})

	libs.glm:setupDep()
	libs.entt:setupDep()
	libs.spdlog:setupDep()
end

function brainfryer:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/Inc/" })

	libs.glm:setupDep()
	libs.entt:setupDep()
	libs.spdlog:setupDep()
end